#include "pluginactionlineage.h"
#include <QJsonArray>

PluginActionLineage::PluginActionLineage(QSharedPointer<const PluginAction> pluginAction) :
    m_pluginAction(pluginAction),
    m_outputPosition(0)
{
}

void PluginActionLineage::recordLineage(
        QSharedPointer<const PluginAction> pluginAction,
        QList<QSharedPointer<BitContainer>> inputContainers,
        QList<QSharedPointer<BitContainer>> outputContainers)
{
    QList<QWeakPointer<const PluginActionLineage>> outputGroup;

    int outputPosition = 0;
    for (auto output: outputContainers) {
        auto lineage = (new PluginActionLineage(pluginAction))->setOutputPosition(outputPosition++);
        for (auto input: inputContainers) {
            lineage->addInput(input->getActionLineage());
        }
        output->setActionLineage(lineage);
        outputGroup.append(lineage.toWeakRef());
    }

    for (auto input: inputContainers) {
        if (input->getActionLineage().isNull()) {
            continue;
        }
        input->getActionLineage()->addOutputGroup(outputGroup);
    }
}

QSharedPointer<PluginActionLineage> PluginActionLineage::setOutputPosition(int outputPosition)
{
    m_outputPosition = 0;
    return sharedFromThis();
}

QSharedPointer<PluginActionLineage> PluginActionLineage::addInput(
        QSharedPointer<const PluginActionLineage> input)
{
    m_inputs.append(input);
    return sharedFromThis();
}

QSharedPointer<const PluginAction> PluginActionLineage::getPluginAction() const
{
    return m_pluginAction;
}

int PluginActionLineage::getOutputPosition() const
{
    return m_outputPosition;
}

QList<QSharedPointer<const PluginActionLineage>> PluginActionLineage::getInputs() const
{
    return m_inputs;
}

QList<QList<QWeakPointer<const PluginActionLineage>>> PluginActionLineage::getOutputs() const
{
    return m_outputs;
}

QJsonObject PluginActionLineage::serialize() const
{
    QJsonObject obj;
    QJsonArray actions;
    for (auto action : getLineage()) {
        QJsonObject actionObject;
        actionObject.insert("action", action->m_pluginAction->serialize());
        actionObject.insert("outputPosition", action->m_outputPosition);
        QJsonArray additionalInputs;
        for (auto input : m_additionalInputs) {
            if (input.isNull()) {
                additionalInputs.append(QJsonObject());
            }
            else {
                additionalInputs.append(input->serialize());
            }
        }
        actionObject.insert("additionalInputs", additionalInputs);
        actions.append(actionObject);
    }
    obj.insert("actions", actions);
    return obj;
}

QSharedPointer<PluginActionLineage> PluginActionLineage::deserialize(QJsonObject data)
{
    QSharedPointer<PluginActionLineage> nullLineage;

    if (!(data.contains("actions")
          && data.value("actions").isArray())) {
        return nullLineage;
    }

    QSharedPointer<PluginActionLineage> lineage;
    for (auto action : data.value("actions").toArray()) {
        if (!action.isObject()) {
            return nullLineage;
        }
        QJsonObject actionObject = action.toObject();
        if (!(actionObject.contains("action")
              && actionObject.value("action").isObject()
              && actionObject.contains("outputPosition")
              && actionObject.value("outputPosition").isDouble())) {
            return nullLineage;
        }
        QSharedPointer<PluginAction> pluginAction = PluginAction::deserialize(actionObject.value("action").toObject());
        if (pluginAction.isNull()) {
            return nullLineage;
        }
        int outputPosition = int(actionObject.value("outputPosition").toDouble());

        lineage = PluginActionLineage::create(pluginAction)->setParent(lineage)->setOutputPos(outputPosition);

        if (actionObject.contains("additionalInputs")
            && actionObject.value("additionalInputs").isArray()) {
            for (auto additional : actionObject.value("additionalInputs").toArray()) {
                if (!additional.isObject()) {
                    return nullLineage;
                }
                QJsonObject additionalObject = additional.toObject();
                if (additionalObject.size() == 0) {
                    lineage->addAdditionalInput(QSharedPointer<PluginActionLineage>());
                }
                else {
                    lineage->addAdditionalInput(PluginActionLineage::deserialize(additionalObject));
                }
            }
        }
    }

    return lineage;
}

QList<QSharedPointer<const PluginActionLineage>> PluginActionLineage::getLineage() const
{
    QList<QSharedPointer<const PluginActionLineage>> lineage;
    if (!m_parent.isNull()) {
        lineage = m_parent->getLineage();
    }
    lineage.append(this->sharedFromThis());
    return lineage;
}

int PluginActionLineage::additionalInputCount() const
{
    int count = 0;
    for (auto lineage : getLineage()) {
        count += lineage->getAdditionalInputs().size();
    }
    return count;
}
