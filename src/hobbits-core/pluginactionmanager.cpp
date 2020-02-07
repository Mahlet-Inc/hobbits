#include "pluginactionlineage.h"
#include "pluginactionmanager.h"
#include "settingsmanager.h"

PluginActionManager::PluginActionManager(QSharedPointer<const PluginManager> pluginManager) :
    QObject(nullptr),
    m_pluginManager(pluginManager),
    m_operatorActor(new OperatorActor(this, pluginManager)),
    m_analyzerActor(new AnalyzerActor(this, pluginManager)),
    m_activeLineage(false),
    m_lineageStep(0)
{
    connect(m_operatorActor.data(), SIGNAL(reportError(QString)), this, SIGNAL(reportError(QString)));
    connect(m_analyzerActor.data(), SIGNAL(reportError(QString)), this, SIGNAL(reportError(QString)));
}

bool PluginActionManager::isBusy() const
{
    return !(m_currOperatorWatcher.isNull()
             && m_currAnalyzerWatcher.isNull());
}

bool PluginActionManager::hasActiveLineage() const
{
    return m_activeLineage;
}

void PluginActionManager::cancelAction()
{
    if (!m_currOperatorWatcher.isNull()) {
        m_currOperatorWatcher->progress()->setCancelled(true);
        m_lineageQueue.clear();
    }
    else if (!m_currAnalyzerWatcher.isNull()) {
        m_currAnalyzerWatcher->progress()->setCancelled(true);
        m_lineageQueue.clear();
    }
}

bool PluginActionManager::registerOperatorWatcher(
        QSharedPointer<ActionWatcher<QSharedPointer<const OperatorResult>>> watcher)
{
    if (this->isBusy()) {
        return false;
    }
    m_currOperatorWatcher = watcher;
    connect(m_currOperatorWatcher->watcher(), SIGNAL(finished()), this, SLOT(operatorWatcherFinished()));
    connect(
            m_currOperatorWatcher->progress().data(),
            SIGNAL(progressPercentChanged(int)),
            this,
            SIGNAL(actionWatcherProgress(int)));
    emit actionWatcherStarted();
    return true;
}

bool PluginActionManager::registerAnalyzerWatcher(
        QSharedPointer<ActionWatcher<QSharedPointer<const AnalyzerResult>>> watcher)
{
    if (this->isBusy()) {
        return false;
    }
    m_currAnalyzerWatcher = watcher;
    connect(m_currAnalyzerWatcher->watcher(), SIGNAL(finished()), this, SLOT(analyzerWatcherFinished()));
    connect(
            m_currAnalyzerWatcher->progress().data(),
            SIGNAL(progressPercentChanged(int)),
            this,
            SIGNAL(actionWatcherProgress(int)));
    emit actionWatcherStarted();
    return true;
}

void PluginActionManager::applyLineage(
        QUuid containerId,
        QSharedPointer<const PluginActionLineage> lineage,
        QSharedPointer<BitContainerManager> bitContainerManager,
        QString baseName,
        QMap<int, QUuid> lineageOverride)
{
    if (m_activeLineage) {
        QSharedPointer<PluginActionManager::LineageAction> actionToQueue =
            QSharedPointer<PluginActionManager::LineageAction>(new PluginActionManager::LineageAction());
        actionToQueue->containerId = containerId;
        actionToQueue->lineage = lineage;
        actionToQueue->bitContainerManager = bitContainerManager;
        actionToQueue->baseName = baseName;
        actionToQueue->outputIdOverride = lineageOverride;
        m_lineageQueue.append(actionToQueue);
    }
    else {
        QSharedPointer<BitContainer> container = bitContainerManager->getContainerById(containerId);
        if (container.isNull()) {
            emit reportError(QString(
                    "Could not apply planned actions - could not find a bit container with the specified ID"));
            finishLineage();
            return;
        }
        m_activeLineage = true;
        m_currLineage = lineage->getLineage();
        m_lineageStep = 0;
        m_lineageBitContainerManager = bitContainerManager;
        m_lineageBaseName = baseName;
        m_currLineageContainers.clear();
        m_currLineageContainers.append(container);
        m_currOutputIdOverride = lineageOverride;
        this->continueLineage();
    }
}

void PluginActionManager::finishLineage()
{
    m_activeLineage = false;
    if (m_lineageQueue.isEmpty()) {
        emit lineageQueueCompleted();
        return;
    }
    QSharedPointer<PluginActionManager::LineageAction> queuedAction = m_lineageQueue.takeFirst();
    applyLineage(
            queuedAction->containerId,
            queuedAction->lineage,
            queuedAction->bitContainerManager,
            queuedAction->baseName,
            queuedAction->outputIdOverride);
}

QSharedPointer<OperatorActor> PluginActionManager::operatorActor()
{
    return m_operatorActor;
}

QSharedPointer<AnalyzerActor> PluginActionManager::analyzerActor()
{
    return m_analyzerActor;
}

void PluginActionManager::operatorWatcherFinished()
{
    disconnect(m_currOperatorWatcher->watcher(), SIGNAL(finished()), this, SLOT(operatorWatcherFinished()));
    disconnect(
            m_currOperatorWatcher->progress().data(),
            SIGNAL(progressPercentChanged(int)),
            this,
            SIGNAL(actionWatcherProgress(int)));

    SettingsManager::getInstance().setPrivateSetting(SettingsData::PLUGIN_RUNNING_KEY, QVariant());
    emit actionWatcherFinished();

    auto result = m_currOperatorWatcher->result();
    m_currOperatorWatcher = QSharedPointer<ActionWatcher<QSharedPointer<const OperatorResult>>>();

    if (m_activeLineage) {
        if (result.isNull() || result->getPluginState().contains("error")) {
            emit reportError(QString("Plugin execution failed, aborting any planned executions"));
            finishLineage();
        }
        else {
            m_currLineageContainers = result->getOutputContainers();
            continueLineage();
        }
    }
}

void PluginActionManager::analyzerWatcherFinished()
{
    disconnect(m_currAnalyzerWatcher->watcher(), SIGNAL(finished()), this, SLOT(analyzerWatcherFinished()));
    disconnect(
            m_currAnalyzerWatcher->progress().data(),
            SIGNAL(progressPercentChanged(int)),
            this,
            SIGNAL(actionWatcherProgress(int)));

    SettingsManager::getInstance().setPrivateSetting(SettingsData::PLUGIN_RUNNING_KEY, QVariant());
    emit actionWatcherFinished();

    auto result = m_currAnalyzerWatcher->result();
    m_currAnalyzerWatcher = QSharedPointer<ActionWatcher<QSharedPointer<const AnalyzerResult>>>();

    if (m_activeLineage) {
        if (result.isNull() || result->getPluginState().contains("error")) {
            emit reportError(QString("Plugin execution failed, aborting any planned executions"));
            finishLineage();
        }
        continueLineage();
    }
}

void PluginActionManager::continueLineage()
{
    if (m_lineageStep >= m_currLineage.size() || m_lineageStep < 0) {
        finishLineage();
        return;
    }

    auto lineageStep = m_currLineage.at(m_lineageStep);
    m_lineageStep++;

    if (lineageStep->getPluginAction()->getPluginType() == PluginAction::Framer) {
        emit reportError(QString("Failed to initiate '%1' step - plugin type '%2' (Framer) is deprecated").arg(
                lineageStep->getPluginAction()->getPluginName()).arg(lineageStep->getPluginAction()->getPluginType()));
        finishLineage();
        return;
    }
    else if (lineageStep->getPluginAction()->getPluginType() == PluginAction::Analyzer) {
        if (!(lineageStep->getOutputPosition() >= 0
              && lineageStep->getOutputPosition() < m_currLineageContainers.size())) {
            emit reportError(QString("Failed to initiate '%1' step - required output container is unavailable").arg(
                    lineageStep->getPluginAction()->getPluginName()));
            finishLineage();
            return;
        }
        auto watcher = lineageStep->getPluginAction()->analyzerAct(
                m_analyzerActor,
                m_pluginManager,
                m_currLineageContainers.at(0));
        if (watcher.isNull()) {
            emit reportError(QString("Failed to run '%1' step - plugin failed to initialize on the given state").arg(
                    lineageStep->getPluginAction()->getPluginName()));
            finishLineage();
        }
    }
    else if (lineageStep->getPluginAction()->getPluginType() == PluginAction::Operator) {
        if (lineageStep->getOutputPosition() >= m_currLineageContainers.size()) {
            emit reportError(QString("Failed to initiate '%1' step - required output container is unavailable").arg(
                    lineageStep->getPluginAction()->getPluginName()));
            finishLineage();
            return;
        }
        // TODO multiple parents
        if (lineageStep->getOutputPosition() == -1) {
            m_currLineageContainers.clear();
        }
        else {
            QSharedPointer<BitContainer> parent = m_currLineageContainers.at(lineageStep->getOutputPosition());
            m_currLineageContainers.clear();
            m_currLineageContainers.append(parent);
        }

// QList<QSharedPointer<const BitContainer> > constContainers;
// for (auto container : m_currLineageContainers) {
// constContainers.append(container);
// }

        QMap<int, QUuid> outputOverride;
        bool finalOp = true;
        for (int i = m_lineageStep; i < m_currLineage.size(); i++) {
            if (m_currLineage.at(i)->getPluginAction()->getPluginType() == PluginAction::Operator) {
                finalOp = false;
                break;
            }
        }
        if (finalOp) {
            outputOverride = m_currOutputIdOverride;
        }
        auto watcher = lineageStep->getPluginAction()->operatorAct(
                m_operatorActor,
                m_pluginManager,
                m_currLineageContainers,
                m_lineageBitContainerManager,
                QString(),
                outputOverride);
        if (watcher.isNull()) {
            emit reportError(QString("Failed to run '%1' step - plugin failed to initialize on the given state").arg(
                    lineageStep->getPluginAction()->getPluginName()));
            finishLineage();
        }
    }
    else {
        emit reportError(QString("Failed to initiate '%1' step - plugin type '%2' is unknown").arg(
                lineageStep->getPluginAction()->getPluginName()).arg(lineageStep->getPluginAction()->getPluginType()));
        finishLineage();
        return;
    }
}
