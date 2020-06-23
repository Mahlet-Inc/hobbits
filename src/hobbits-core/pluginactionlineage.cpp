#include "pluginactionlineage.h"
#include <QJsonArray>
#include <QQueue>

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

    for (auto input: inputContainers) {
        if (input->getActionLineage().isNull()) {
            input->setActionLineage(actionlessLineage());
        }
    }

    int outputPosition = 0;
    for (auto output: outputContainers) {
        QSharedPointer<PluginActionLineage> lineage(new PluginActionLineage(pluginAction));
        lineage->setOutputPosition(outputPosition++);
        for (auto input: inputContainers) {
            lineage->addInput(input->getActionLineage());
        }
        output->setActionLineage(lineage);
        outputGroup.append(lineage.toWeakRef());
    }

    for (auto input: inputContainers) {
        input->getActionLineage()->addOutputGroup(outputGroup);
    }
}


QSharedPointer<PluginActionLineage> PluginActionLineage::actionlessLineage()
{
    return QSharedPointer<PluginActionLineage>(new PluginActionLineage(PluginAction::noAction()));
}

QSharedPointer<PluginActionLineage> PluginActionLineage::setOutputPosition(int outputPosition)
{
    m_outputPosition = outputPosition;
    return this->sharedFromThis();
}

QSharedPointer<PluginActionLineage> PluginActionLineage::addInput(
        QSharedPointer<const PluginActionLineage> input)
{
    m_inputs.append(input);
    return this->sharedFromThis();
}

QSharedPointer<PluginActionLineage> PluginActionLineage::addOutputGroup(QList<QWeakPointer<const PluginActionLineage>> outputs)
{
    m_outputs.append(outputs);
    return this->sharedFromThis();
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

QSharedPointer<const PluginAction> PluginActionLineage::containerSourceAnalyzer() const
{
    for (auto input: m_inputs) {
        if (input->getPluginAction()->getPluginType() == PluginAction::Analyzer) {
            return input->getPluginAction();
        }
    }

    return QSharedPointer<const PluginAction>();
}

QSharedPointer<const PluginAction> PluginActionLineage::containerSourceOperator() const
{
    QList<QSharedPointer<const PluginActionLineage>> inputs = getInputs();
    while (!inputs.isEmpty()) {
        auto input = inputs.takeFirst();
        if (input->getPluginAction()->getPluginType() == PluginAction::Operator) {
            return input->getPluginAction();
        }
        else if (input->getPluginAction()->getPluginType() == PluginAction::Analyzer) {
            inputs.append(input->getInputs());
        }
    }

    return QSharedPointer<const PluginAction>();
}
