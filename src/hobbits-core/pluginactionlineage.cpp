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
