#include "operatorrunner.h"
#include "pluginaction.h"
#include "pluginactionlineage.h"
#include "pluginactionmanager.h"
#include "settingsmanager.h"
#include <QTimer>

OperatorRunner::OperatorRunner(QString pluginName, QString pluginFileLocation) :
    AbstractPluginRunner<const OperatorResult>(pluginName, pluginFileLocation)
{

}

QSharedPointer<OperatorRunner> OperatorRunner::create(
        QSharedPointer<const HobbitsPluginManager> pluginManager,
        QSharedPointer<BitContainerManager> containerManager,
        QSharedPointer<const PluginAction> action)
{
    if (action->pluginType() != PluginAction::Operator) {
        return nullptr;
    }
    auto plugin = pluginManager->getOperator(action->pluginName());
    if (!plugin) {
        return nullptr;
    }

    auto runner = QSharedPointer<OperatorRunner>(new OperatorRunner(plugin->name(), pluginManager->getPluginLocation(plugin->name())));

    runner->m_op = plugin;
    runner->m_action = action;
    runner->m_containerManager = containerManager;

    return runner;
}

QSharedPointer<PluginActionWatcher<QSharedPointer<const OperatorResult>>> OperatorRunner::run(QList<QSharedPointer<BitContainer>> inputContainers)
{
    auto parameters = m_action->parameters();
    if (!commonPreRun(parameters)) {
        return QSharedPointer<PluginActionWatcher<QSharedPointer<const OperatorResult>>>();
    }

    QList<QSharedPointer<const BitContainer>> inputContainersConst;
    for (QSharedPointer<BitContainer> input : inputContainers) {
        inputContainersConst.append(input);
    }

    QSharedPointer<PluginActionProgress> progress(new PluginActionProgress());
    auto future = QtConcurrent::run(
            QThreadPool::globalInstance(),
            OperatorRunner::operatorCall,
            m_op,
            inputContainersConst,
            parameters,
            progress);


    m_inputContainers = inputContainers;
    return commonRunSetup(future, progress);
}

void OperatorRunner::postProcess()
{

    if (!commonPostRun()) {
        return;
    }

    // Apply action lineage
    if (!m_result->hasEmptyParameters()) {
        QSharedPointer<PluginAction> action =
            QSharedPointer<PluginAction>(
                    new PluginAction(
                            PluginAction::Operator,
                            m_op->name(),
                            m_result->parameters()));
        PluginActionLineage::recordLineage(action, m_inputContainers, m_result->outputContainers());
    }

    // Set Parent/Child Relationships
    for (QSharedPointer<BitContainer> inputContainer : m_inputContainers) {
        for (QSharedPointer<BitContainer> outputContainer : m_result->outputContainers()) {
            inputContainer->addChild(outputContainer->id());
            outputContainer->addParent(inputContainer->id());
        }
    }

    // Add output containers to container manager
    if (m_result->outputContainers().size() > 0) {
        int number = 1;
        for (QSharedPointer<BitContainer> output : m_result->outputContainers()) {
            QString containerName = m_op->name() + " Output";
            if (output->nameWasSet()) {
                containerName = output->name();
            }
            if (m_result->outputContainers().length() > 1) {
                output->setName(QString("%2: %1").arg(containerName).arg(number));
            }
            else {
                output->setName(containerName);
            }
            number++;
        }
    }

    if (!m_containerManager.isNull()) {
        QSharedPointer<BitContainer> lastContainer;
        for (QSharedPointer<BitContainer> output : m_result->outputContainers()) {
            if (m_containerManager->addContainer(output)) {
                lastContainer = output;
            }
        }
        if (!lastContainer.isNull()) {
            m_containerManager->selectContainer(lastContainer);
        }
    }

    emit finished(m_id);
}

QSharedPointer<const OperatorResult> OperatorRunner::operatorCall(
        QSharedPointer<OperatorInterface> op,
        QList<QSharedPointer<const BitContainer>> inputContainers,
        QJsonObject parameters,
        QSharedPointer<PluginActionProgress> progressTracker)
{
    try {
        return op->operateOnBits(inputContainers, parameters, progressTracker);
    } catch (std::exception &e) {
        return OperatorResult::error(QString("Exception encountered in plugin %1: %2").arg(op->name()).arg(e.what()));
    } catch (...) {
        return OperatorResult::error(QString("Unexpected exception in plugin %1").arg(op->name()));
    }
}
