#include "analyzeractor.h"
#include "pluginaction.h"
#include "pluginactionlineage.h"
#include "pluginactionmanager.h"
#include "settingsmanager.h"

AnalyzerActor::AnalyzerActor(PluginActionManager *manager, QSharedPointer<const HobbitsPluginManager> pluginManager) :
    QObject(),
    m_pluginManager(pluginManager),
    m_manager(manager)
{
}

QSharedPointer<ActionWatcher<QSharedPointer<const AnalyzerResult>>> AnalyzerActor::analyzerFullAct(
        QSharedPointer<AnalyzerInterface> analyzer,
        QSharedPointer<BitContainer> container,
        QJsonObject pluginState)
{

    if (m_future.isRunning() || m_manager->isBusy()) {
        return QSharedPointer<ActionWatcher<QSharedPointer<const AnalyzerResult>>>();
    }

    if (pluginState.isEmpty()) {
        pluginState = analyzer->getStateFromUi();
        if (pluginState.isEmpty() || pluginState.contains("error")) {
            if (pluginState.contains("error")) {
                emit reportError(QString("Plugin '%1' reported an error with its current state: '%2'").arg(
                        analyzer->getName()).arg(pluginState.value("error").toString()));
            }
            else if (pluginState.isEmpty()) {
                emit reportError(QString(
                        "Plugin '%1' is in an invalid state and can't be executed.  Double check the input fields.").arg(
                                         analyzer->getName()));
            }
            return QSharedPointer<ActionWatcher<QSharedPointer<const AnalyzerResult>>>();
        }
    }

    QSharedPointer<ActionProgress> progress(new ActionProgress());

    SettingsManager::getInstance().setPrivateSetting(
            SettingsData::PLUGIN_RUNNING_KEY,
            QVariant(m_pluginManager->getPluginLocation(analyzer->getName())));

    m_future = QtConcurrent::run(
            QThreadPool::globalInstance(),
            AnalyzerActor::analyzerCall,
            analyzer,
            container,
            pluginState,
            progress);

    m_actionWatcher = QSharedPointer<ActionWatcher<QSharedPointer<const AnalyzerResult>>>(
            new ActionWatcher<QSharedPointer<const AnalyzerResult>>(
                    m_future,
                    progress));
    m_analyzer = analyzer;
    m_container = container;
    m_pluginState = pluginState;

    connect(m_actionWatcher->watcher(), SIGNAL(finished()), this, SLOT(postProcess()));

    m_manager->registerAnalyzerWatcher(m_actionWatcher);

    return m_actionWatcher;
}

void AnalyzerActor::postProcess()
{
    disconnect(m_actionWatcher->watcher(), SIGNAL(finished()), this, SLOT(postProcess()));

    QSharedPointer<const AnalyzerResult> result = m_future.result();

    if (result.isNull()) {
        emit reportError(QString("Plugin '%1' failed to execute.  Double check the input fields.").arg(
                m_analyzer->getName()));
        return;
    }

    if (result->getPluginState().contains("error")) {
        emit reportError(QString("Plugin '%1' reported an error with its processing: %2").arg(m_analyzer->getName()).arg(
                result->getPluginState().value("error").toString()));
        return;
    }

    QSharedPointer<PluginAction> action =
        QSharedPointer<PluginAction>(
                new PluginAction(
                        PluginAction::Analyzer,
                        m_analyzer->getName(),
                        result->getPluginState()));
    QSharedPointer<PluginActionLineage> lineage = PluginActionLineage::create(action)->setParent(
            m_container->getActionLineage());

    m_container->setActionLineage(lineage);

    if (!result->bitInfo().isNull()) {
        m_container->setBitInfo(result->bitInfo());
    }
}

QSharedPointer<const AnalyzerResult> AnalyzerActor::analyzerCall(
        QSharedPointer<AnalyzerInterface> analyzer,
        QSharedPointer<const BitContainer> bits,
        QJsonObject pluginState,
        QSharedPointer<ActionProgress> progressTracker)
{
    return analyzer->analyzeBits(bits, pluginState, progressTracker);
}
