#include "operatoractor.h"
#include "pluginaction.h"
#include "pluginactionlineage.h"
#include "pluginactionmanager.h"
#include "settingsmanager.h"

OperatorActor::OperatorActor(PluginActionManager *manager, QSharedPointer<const PluginManager> pluginManager) :
    QObject(),
    m_pluginManager(pluginManager),
    m_manager(manager)
{
}

QSharedPointer<ActionWatcher<QSharedPointer<const OperatorResult>>> OperatorActor::operatorFullAct(
        QSharedPointer<OperatorInterface> op,
        QList<QSharedPointer<BitContainer>> inputContainers,
        QSharedPointer<BitContainerManager> bitContainerManager,
        QString outputName,
        QJsonObject pluginState,
        QMap<int, QUuid> outputIdMap)
{

    if (m_future.isRunning() || m_manager->isBusy()) {
        return QSharedPointer<ActionWatcher<QSharedPointer<const OperatorResult>>>();
    }

    QList<QSharedPointer<const BitContainer>> inputContainersConst;
    for (QSharedPointer<BitContainer> input : inputContainers) {
        inputContainersConst.append(input);
    }

    if (pluginState.isEmpty()) {
        pluginState = op->getStateFromUi();
        if (pluginState.isEmpty() || pluginState.contains("error")) {
            if (pluginState.contains("error")) {
                emit reportError(QString("Plugin '%1' reported an error with its current state: '%2'").arg(
                        op->getName()).arg(pluginState.value("error").toString()));
            }
            else if (pluginState.isEmpty()) {
                emit reportError(QString(
                        "Plugin '%1' is in an invalid state and can't be excecuted.  Double check the input fields.").arg(
                                         op->getName()));
            }
            return QSharedPointer<ActionWatcher<QSharedPointer<const OperatorResult>>>();
        }
    }

    QSharedPointer<ActionProgress> progress(new ActionProgress());

    SettingsManager::getInstance().setPrivateSetting(
            SettingsData::PLUGIN_RUNNING_KEY,
            QVariant(m_pluginManager->getPluginLocation(op->getName())));

    m_future = QtConcurrent::run(
            QThreadPool::globalInstance(),
            OperatorActor::operatorCall,
            op,
            inputContainersConst,
            pluginState,
            progress);

    m_actionWatcher = QSharedPointer<ActionWatcher<QSharedPointer<const OperatorResult>>>(
            new ActionWatcher<QSharedPointer<const OperatorResult>>(
                    m_future,
                    progress));
    m_op = op;
    m_inputContainers = inputContainers;
    m_outputName = outputName;
    m_bitContainerManager = bitContainerManager;
    m_pluginState = pluginState;
    m_outputIdMap = outputIdMap;

    connect(m_actionWatcher->watcher(), SIGNAL(finished()), this, SLOT(postProcess()));

    m_manager->registerOperatorWatcher(m_actionWatcher);

    return m_actionWatcher;
}

void OperatorActor::postProcess()
{
    disconnect(m_actionWatcher->watcher(), SIGNAL(finished()), this, SLOT(postProcess()));

    QSharedPointer<const OperatorResult> result = m_future.result();

    if (result.isNull()) {
        emit reportError(QString("Plugin '%1' failed to execute.  Double check the input fields.").arg(m_op->getName()));
        return;
    }

    if (result->getPluginState().contains("error")) {
        emit reportError(QString("Plugin '%1' reported an error with its processing: %2").arg(m_op->getName()).arg(
                result->getPluginState().value("error").toString()));
        return;
    }

    // Apply action lineage
    if (!result->getPluginState().isEmpty()) {
        for (int i = 0; i < result->getOutputContainers().size(); i++) {
            QSharedPointer<PluginAction> action =
                QSharedPointer<PluginAction>(
                        new PluginAction(
                                PluginAction::Operator,
                                m_op->getName(),
                                result->getPluginState()));
            QSharedPointer<PluginActionLineage> lineage = PluginActionLineage::create(action)->setOutputPos(i);
            if (!m_inputContainers.isEmpty()) {
                // TODO: multiple parents
                lineage->setParent(m_inputContainers.at(0)->getActionLineage());
            }
            result->getOutputContainers().at(i)->setActionLineage(lineage);
        }
    }

    // Adjust the output containers' IDs if they are being forced
    for (int i = 0; i < result->getOutputContainers().size(); i++) {
        if (m_outputIdMap.contains(i)) {
            result->getOutputContainers().at(i)->m_id = m_outputIdMap.value(i);
        }
    }
    m_outputIdMap.clear();

    // Set Parent/Child Relationships
    for (QSharedPointer<BitContainer> inputContainer : m_inputContainers) {
        for (QSharedPointer<BitContainer> outputContainer : result->getOutputContainers()) {
            inputContainer->addChild(outputContainer->getId());
            outputContainer->addParent(inputContainer->getId());
        }
    }

    // Add output containers to container manager
    if (result->getOutputContainers().size() > 0) {
        if (m_outputName.isEmpty()) {
            if (result->getPluginState().contains("container_name")
                && result->getPluginState().value("container_name").isString()) {
                m_outputName = result->getPluginState().value("container_name").toString();
            }
            else {
                m_outputName = m_op->getName() + " Output";
            }
        }
        int number = 1;
        QModelIndex lastAdded;
        for (QSharedPointer<BitContainer> output : result->getOutputContainers()) {
            if (result->getOutputContainers().length() > 1) {
                output->setName(QString("%2: %1").arg(m_outputName).arg(number));
            }
            else {
                output->setName(m_outputName);
            }
            number++;
            lastAdded = m_bitContainerManager->getTreeModel()->addContainer(output);
        }
        if (lastAdded.isValid()) {
            m_bitContainerManager->getCurrSelectionModel()->setCurrentIndex(
                    lastAdded,
                    QItemSelectionModel::ClearAndSelect);
        }
    }
}

QSharedPointer<const OperatorResult> OperatorActor::operatorCall(
        QSharedPointer<OperatorInterface> op,
        QList<QSharedPointer<const BitContainer>> inputContainers,
        QJsonObject pluginState,
        QSharedPointer<ActionProgress> progressTracker)
{
    return op->operateOnContainers(inputContainers, pluginState, progressTracker);
}
