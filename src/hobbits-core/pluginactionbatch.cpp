#include "pluginactionbatch.h"
#include <QJsonArray>

PluginActionBatch::PluginActionBatch(QList<QSharedPointer<const ActionStep>> actionSteps) :
    m_actionSteps(actionSteps)
{

}

QSharedPointer<PluginActionBatch> PluginActionBatch::fromLineage(QSharedPointer<const PluginActionLineage> lineage, int batchMode)
{
    // Stage One: Fill Step Map/Index
    // The actions around the target PluginActionLineage can have weird branching and converging dependencies.
    // The steps need to be created and assigned ids before the input step ids can be easily determined
    QHash<QSharedPointer<const PluginAction>, QSharedPointer<ActionStep>> stepMap;
    QQueue<QPair<QSharedPointer<const PluginActionLineage>, int>> lineageQueue;
    QQueue<QSharedPointer<const PluginActionLineage>> stageTwoQueue;

    lineageQueue.enqueue({lineage, batchMode});
    while (!lineageQueue.isEmpty()) {
        auto lineageModePair = lineageQueue.dequeue();
        auto currLineage = lineageModePair.first;
        auto currMode = lineageModePair.second;
        auto currActionMode = currMode & Mode::ActionModeSegment;

        // If the mode of this lineage is "Inclusive", the ActionStep that produced it will be part of the batch
        if (currMode & Mode::Inclusive) {
            // Multiple PluginActionLineage nodes can refer to different outputs of the same action.
            // We only want one ActionStep per PluginAction
            if (!stepMap.contains(currLineage->getPluginAction())) {
                stageTwoQueue.enqueue(currLineage);
                auto uuid = QUuid::createUuid();
                auto action = currLineage->getPluginAction();
                if (action->getPluginType() == PluginAction::Importer) {
                    if (currMode & Mode::IncludeImporters) {
                        if (!(currMode & Mode::IncludeImporterState)) {
                            action = PluginAction::importerAction(action->getPluginName());
                        }
                    }
                    else {
                        action = PluginAction::noAction();
                    }
                }
                auto actionStep = createStep(uuid, action);
                stepMap.insert(currLineage->getPluginAction(), actionStep);
            }
        }
        // make a "bypass" step so that the outputs can reference a single container
        else if (batchMode & Mode::After) {
            stageTwoQueue.enqueue(currLineage);
            auto noActionStep = createStep(QUuid::createUuid(), PluginAction::noAction());
            stepMap.insert(currLineage->getPluginAction(), noActionStep);
        }

        // If the mode of this lineage is "Before", the ActionSteps leading to its inputs will be part of the batch
        if (batchMode & Mode::Before) {
            for (auto input : currLineage->getInputs()) {
                lineageQueue.append({input, (Mode::InclusiveBefore | currActionMode)});
            }
        }

        // If the mode of this lineage is "After", the ActionSteps leading to its output(s) will be part of the batch
        if (batchMode & Mode::After) {
            for (auto outputSet : currLineage->getOutputs()) {
                for  (auto output: outputSet) {
                    auto outputRef = output.toStrongRef();
                    if (!outputRef.isNull()) {
                        lineageQueue.append({outputRef, (Mode::InclusiveAfter | currActionMode)});
                    }
                }
            }
        }
    }

    // Stage Two: Set Step Inputs
    // Now that every desired ActionStep around the target PluginActionLineage have ids and are indexed,
    // we can iterate through again in order to assign input ids
    QSet<QSharedPointer<const PluginAction>> alreadyAssigned;
    while (!stageTwoQueue.isEmpty()) {
        auto currLineage = stageTwoQueue.dequeue();

        // A single plugin step/action will have the same inputs in all lineages,
        // so we skip steps that already have their inputs assigned
        if (alreadyAssigned.contains(currLineage->getPluginAction())) {
            continue;
        }
        alreadyAssigned.insert(currLineage->getPluginAction());

        // If it's a "no action" step, it will get a null QUuid input
        if (stepMap[currLineage->getPluginAction()]->action->getPluginType() == PluginAction::NoAction) {
            // TODO: what if there's a "no action" that isn't in the beginning?
            stepMap[currLineage->getPluginAction()]->inputs = {{QUuid(), 0}};
            continue;
        }

        // The prerequisite ActionSteps (via their ids) and the correct output position of those steps (via the lineage
        // outputPosition) provide the information necessary to execute the ActionStep with the correct inputs
        QList<QPair<QUuid, int>> inputs;
        for (auto input : currLineage->getInputs()) {
            // The lineage might have inputs that are not included based on the Mode - those inputs are "variables" that
            // will be selected at batch runtime, so they will be marked with null QUuids
            if (stepMap.contains(input->getPluginAction())) {
                inputs.append({stepMap.value(input->getPluginAction())->stepId, input->getOutputPosition()});
            }
            else {
                inputs.append({QUuid(), 0});
            }
        }
        stepMap[currLineage->getPluginAction()]->inputs = inputs;
    }


    QList<QSharedPointer<const ActionStep>> constSteps;
    for (auto step: stepMap.values()) {
        constSteps.append(step);
    }

    return QSharedPointer<PluginActionBatch>(new PluginActionBatch(constSteps));
}


QSharedPointer<PluginActionBatch::ActionStep> PluginActionBatch::createStep(QUuid id, QSharedPointer<const PluginAction> action)
{
    return QSharedPointer<ActionStep>(new ActionStep(id, action));
}

QJsonObject PluginActionBatch::serialize() const
{
    QJsonObject obj;
    QJsonArray steps;
    for (auto step : this->m_actionSteps) {
        QJsonObject stepObject;
        stepObject.insert("id", step->stepId.toString());
        stepObject.insert("action", step->action->serialize());
        QJsonArray stepInputs;
        for (auto input : step->inputs) {
            QJsonObject inputObject;
            if (input.first.isNull()) {
                inputObject.insert("stepId", "");
            }
            else {
                inputObject.insert("stepId", input.first.toString());
            }
            inputObject.insert("outputPosition", input.second);
            stepInputs.append(inputObject);
        }
        stepObject.insert("inputs", stepInputs);
        steps.append(stepObject);
    }
    obj.insert("steps", steps);
    return obj;
}

QSharedPointer<PluginActionBatch> PluginActionBatch::deserialize(QJsonObject data)
{
    QSharedPointer<PluginActionBatch> nullBatch;

    if (!(data.contains("steps")
          && data.value("steps").isArray())) {
        return nullBatch;
    }

    QSharedPointer<PluginActionBatch> deserialized;
    QList<QSharedPointer<ActionStep>> steps;
    for (auto step : data.value("steps").toArray()) {
        if (!step.isObject()) {
            return nullBatch;
        }
        QJsonObject stepObject = step.toObject();
        if (!(stepObject.contains("action")
              && stepObject.value("action").isObject()
              && stepObject.contains("id")
              && stepObject.value("id").isString())) {
            return nullBatch;
        }
        QSharedPointer<const PluginAction> pluginAction = PluginAction::deserialize(stepObject.value("action").toObject());
        if (pluginAction.isNull()) {
            return nullBatch;
        }
        QUuid stepId = QUuid::fromString(stepObject.value("id").toString());

        auto deserializedStep = createStep(stepId, pluginAction);
        steps.append(deserializedStep);

        if (stepObject.contains("inputs")
            && stepObject.value("inputs").isArray()) {
            for (auto input : stepObject.value("inputs").toArray()) {
                if (!input.isObject()) {
                    return nullBatch;
                }
                QJsonObject inputObject = input.toObject();
                if (!(inputObject.contains("outputPosition")
                      && inputObject.value("outputPosition").isDouble()
                      && inputObject.contains("stepId")
                      && inputObject.value("stepId").isString())) {
                    return nullBatch;
                }
                QUuid inputId;
                if (inputObject.value("stepId").toString().isEmpty()) {
                    inputId = QUuid();
                }
                else {
                    inputId = QUuid::fromString(inputObject.value("stepId").toString());
                }
                int outputPosition = int(inputObject.value("outputPosition").toDouble());
                deserializedStep->inputs.append({inputId, outputPosition});
            }
        }
    }

    QList<QSharedPointer<const ActionStep>> constSteps;
    for (auto step: steps) {
        constSteps.append(step);
    }

    return QSharedPointer<PluginActionBatch>(new PluginActionBatch(constSteps));
}

int PluginActionBatch::getMinRequiredInputs(QSharedPointer<const HobbitsPluginManager> pluginManager) const
{
    int inputTotal = 0;
    for (auto step : m_actionSteps) {
        if (step->action->getPluginType() == PluginAction::NoAction) {
            inputTotal += 1;
            continue;
        }
        int actionInputs = step->action->minPossibleInputs(pluginManager);
        int internalInputs = 0;
        for (auto input : step->inputs) {
            if (!input.first.isNull()) {
                internalInputs++;
            }
        }
        if (actionInputs > internalInputs) {
            inputTotal += actionInputs - internalInputs;
        }
    }
    return inputTotal;
}

int PluginActionBatch::getMaxPossibleInputs(QSharedPointer<const HobbitsPluginManager> pluginManager) const
{
    int inputTotal = 0;
    for (auto step : m_actionSteps) {
        int actionInputs = step->action->maxPossibleInputs(pluginManager);
        int internalInputs = 0;
        for (auto input : step->inputs) {
            if (!input.first.isNull()) {
                internalInputs++;
            }
        }
        if (actionInputs > internalInputs) {
            inputTotal += actionInputs - internalInputs;
        }
    }
    return inputTotal;
}

QList<QSharedPointer<const PluginActionBatch::ActionStep>> PluginActionBatch::actionSteps() const
{
    return m_actionSteps;
}
