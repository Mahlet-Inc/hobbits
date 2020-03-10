#include "pluginactionlineage.h"
#include "pluginactionmanager.h"
#include "settingsmanager.h"

PluginActionManager::PluginActionManager(QSharedPointer<const PluginManager> pluginManager) :
    QObject(nullptr),
    m_pluginManager(pluginManager),
    m_operatorActor(new OperatorActor(this, pluginManager)),
    m_analyzerActor(new AnalyzerActor(this, pluginManager))
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
    return !m_current.isNull();
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
        QMap<int, QUuid> lineageOverride,
        QList<QUuid> additionalInputs)
{
    QSharedPointer<PluginActionManager::LineageAction> action =
        QSharedPointer<PluginActionManager::LineageAction>(new PluginActionManager::LineageAction());
    action->containerId = containerId;
    action->lineage = lineage->getLineage();
    action->bitContainerManager = bitContainerManager;
    action->baseName = baseName;
    action->outputIdOverride = lineageOverride;
    action->additionalInputIds = additionalInputs;
    action->step = 0;
    action->additionalStep = 0;
    applyLineage(action);
}

void PluginActionManager::applyLineage(QSharedPointer<PluginActionManager::LineageAction> lineage)
{
    if (hasActiveLineage()) {
        m_lineageQueue.append(lineage);
    }
    else {
        m_current = lineage;
        QSharedPointer<BitContainer> container = lineage->bitContainerManager->getContainerById(lineage->containerId);
        if (container.isNull()) {
            emit reportError(QString(
                    "Could not apply planned actions - could not find a bit container with the specified ID"));
            finishLineage();
            return;
        }

        m_current->inputs.clear();
        m_current->inputs.append(container);
        this->continueLineage();
    }
}

void PluginActionManager::finishLineage()
{
    if (!m_lineageStack.isEmpty()) {
        auto parent = m_lineageStack.pop();
        if (!m_current->inputs.isEmpty()) {
            parent->additionalInputs.append(m_current->inputs.at(0));
        }
        parent->additionalStep++;
        parent->additionalInputIds = m_current->additionalInputIds;
        m_current = parent;
        this->continueLineage();
        return;
    }

    m_current = QSharedPointer<PluginActionManager::LineageAction>();
    if (m_lineageQueue.isEmpty()) {
        emit lineageQueueCompleted();
        return;
    }
    QSharedPointer<PluginActionManager::LineageAction> queuedAction = m_lineageQueue.takeFirst();
    applyLineage(queuedAction);
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

    if (hasActiveLineage()) {
        if (result.isNull() || result->getPluginState().contains("error")) {
            emit reportError(QString("Plugin execution failed, aborting any planned executions"));
            finishLineage();
        }
        else {
            m_current->inputs = result->getOutputContainers();
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

    if (hasActiveLineage()) {
        if (result.isNull() || result->getPluginState().contains("error")) {
            emit reportError(QString("Plugin execution failed, aborting any planned executions"));
            finishLineage();
        }
        continueLineage();
    }
}

void PluginActionManager::continueLineage()
{
    if (m_current->step < 0) {
        emit reportError("Plugin action manager in invalid state: negative lineage step");
        finishLineage();
        return;
    }

    if (m_current->step >= m_current->lineage.size()) {
        finishLineage();
        return;
    }

    auto lineageStep = m_current->lineage.at(m_current->step);
    if (lineageStep->getAdditionalInputs().size() > m_current->additionalStep) {
        if (m_current->additionalInputIds.isEmpty()) {
            emit reportError(QString("Not enough additional inputs provided to execute step %1 - %2").arg(m_current->step).arg(lineageStep->getPluginAction()->getPluginName()));
            finishLineage();
            return;
        }
        m_lineageStack.push(m_current);
        QSharedPointer<PluginActionManager::LineageAction> action =
            QSharedPointer<PluginActionManager::LineageAction>(new PluginActionManager::LineageAction());
        action->containerId = m_current->additionalInputIds.takeFirst();
        if (!lineageStep->getAdditionalInputs().at(m_current->additionalStep).isNull()) {
            action->lineage = lineageStep->getAdditionalInputs().at(m_current->additionalStep)->getLineage();
        }
        action->bitContainerManager = m_current->bitContainerManager;
        action->baseName = m_current->baseName;
        action->additionalInputIds = m_current->additionalInputIds;
        action->step = 0;
        action->additionalStep = 0;
        m_current = QSharedPointer<PluginActionManager::LineageAction>();
        applyLineage(action);
        return;
    }
    m_current->step++;
    m_current->additionalStep = 0;

    if (lineageStep->getPluginAction()->getPluginType() == PluginAction::Framer) {
        emit reportError(QString("Failed to initiate '%1' step - plugin type '%2' (Framer) is deprecated").arg(
                lineageStep->getPluginAction()->getPluginName()).arg(lineageStep->getPluginAction()->getPluginType()));
        finishLineage();
        return;
    }
    else if (lineageStep->getPluginAction()->getPluginType() == PluginAction::Analyzer) {
        if (!(lineageStep->getOutputPosition() >= 0
              && lineageStep->getOutputPosition() < m_current->inputs.size())) {
            emit reportError(QString("Failed to initiate '%1' step - required output container is unavailable").arg(
                    lineageStep->getPluginAction()->getPluginName()));
            finishLineage();
            return;
        }
        auto watcher = lineageStep->getPluginAction()->analyzerAct(
                m_analyzerActor,
                m_pluginManager,
                m_current->inputs.at(0));
        if (watcher.isNull()) {
            emit reportError(QString("Failed to run '%1' step - plugin failed to initialize on the given state").arg(
                    lineageStep->getPluginAction()->getPluginName()));
            finishLineage();
        }
    }
    else if (lineageStep->getPluginAction()->getPluginType() == PluginAction::Operator) {
        if (lineageStep->getOutputPosition() >= m_current->inputs.size()) {
            emit reportError(QString("Failed to initiate '%1' step - required output container is unavailable").arg(
                    lineageStep->getPluginAction()->getPluginName()));
            finishLineage();
            return;
        }
        if (lineageStep->getOutputPosition() == -1) {
            m_current->inputs.clear();
        }
        else {
            QSharedPointer<BitContainer> parent = m_current->inputs.at(lineageStep->getOutputPosition());
            m_current->inputs.clear();
            m_current->inputs.append(parent);
        }
        for (auto additional: m_current->additionalInputs) {
            m_current->inputs.append(additional);
        }

        QMap<int, QUuid> outputOverride;
        bool finalOp = true;
        for (int i = m_current->step; i < m_current->lineage.size(); i++) {
            if (m_current->lineage.at(i)->getPluginAction()->getPluginType() == PluginAction::Operator) {
                finalOp = false;
                break;
            }
        }
        if (finalOp) {
            outputOverride = m_current->outputIdOverride;
        }
        auto watcher = lineageStep->getPluginAction()->operatorAct(
                m_operatorActor,
                m_pluginManager,
                m_current->inputs,
                m_current->bitContainerManager,
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
