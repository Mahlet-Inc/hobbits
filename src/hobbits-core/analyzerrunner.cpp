#include "analyzerrunner.h"
#include "pluginaction.h"
#include "pluginactionlineage.h"
#include "pluginactionmanager.h"
#include "settingsmanager.h"

QSharedPointer<AnalyzerRunner> AnalyzerRunner::create(
        QSharedPointer<const HobbitsPluginManager> pluginManager,
        QSharedPointer<PluginAction> action)
{
    if (action->getPluginType() != PluginAction::Analyzer) {
        return nullptr;
    }
    auto plugin = pluginManager->getAnalyzer(action->getPluginName());
    if (!plugin) {
        return nullptr;
    }

    auto runner = QSharedPointer<AnalyzerRunner>(new AnalyzerRunner());

    runner->m_id = QUuid::createUuid();
    runner->m_analyzer = plugin;
    runner->m_action = action;
    runner->m_pluginFileLocation = pluginManager->getPluginLocation(plugin->getName());

    return runner;
}

QUuid AnalyzerRunner::id() const
{
    return m_id;
}

QSharedPointer<ActionWatcher<QSharedPointer<const AnalyzerResult>>> AnalyzerRunner::getWatcher()
{
    return m_actionWatcher;
}

QSharedPointer<ActionWatcher<QSharedPointer<const AnalyzerResult>>> AnalyzerRunner::run(QSharedPointer<BitContainer> container)
{

    if (m_actionWatcher->watcher()->future().isRunning()) {
        emit reportError(QString("Analyzer runner '%1' is already running").arg(m_id.toString()));
        return QSharedPointer<ActionWatcher<QSharedPointer<const AnalyzerResult>>>();
    }

    auto pluginState = m_action->getPluginState();
    if (pluginState.isEmpty()) {
        pluginState = m_analyzer->getStateFromUi();
        if (pluginState.isEmpty() || pluginState.contains("error")) {
            if (pluginState.contains("error")) {
                emit reportError(QString("Plugin '%1' reported an error with its current state: '%2'").arg(
                        m_analyzer->getName()).arg(pluginState.value("error").toString()));
            }
            else if (pluginState.isEmpty()) {
                emit reportError(QString(
                        "Plugin '%1' is in an invalid state and can't be executed.  Double check the input fields.").arg(
                                         m_analyzer->getName()));
            }
            return QSharedPointer<ActionWatcher<QSharedPointer<const AnalyzerResult>>>();
        }
    }

    QSharedPointer<ActionProgress> progress(new ActionProgress());

    QVariant previousRunning = SettingsManager::getInstance().getPrivateSetting(SettingsData::PLUGINS_RUNNING_KEY);
    QStringList runningPlugins;
    if (previousRunning.isValid() && previousRunning.canConvert<QStringList>()) {
        runningPlugins = previousRunning.toStringList();
    }
    runningPlugins.append(m_pluginFileLocation);
    SettingsManager::getInstance().setPrivateSetting(SettingsData::PLUGINS_RUNNING_KEY, QVariant(runningPlugins));

    auto future = QtConcurrent::run(
            QThreadPool::globalInstance(),
            AnalyzerRunner::analyzerCall,
            m_analyzer,
            container,
            pluginState,
            progress);

    m_actionWatcher = QSharedPointer<ActionWatcher<QSharedPointer<const AnalyzerResult>>>(
            new ActionWatcher<QSharedPointer<const AnalyzerResult>>(
                    future,
                    progress));

    m_container = container;

    connect(m_actionWatcher->watcher(), SIGNAL(finished()), this, SLOT(postProcess()));

    return m_actionWatcher;
}

void AnalyzerRunner::postProcess()
{
    disconnect(m_actionWatcher->watcher(), SIGNAL(finished()), this, SLOT(postProcess()));

    QSharedPointer<const AnalyzerResult> result = m_actionWatcher->watcher()->future().result();

    if (result.isNull()) {
        QString errorString = QString("Plugin '%1' failed to execute.  Double check the input fields.").arg(
                    m_analyzer->getName());
        emit reportError(errorString);
        emit finishedFail(m_id, errorString);
        return;
    }

    if (result->getPluginState().contains("error")) {
        QString errorString = QString("Plugin '%1' reported an error with its processing: %2").arg(m_analyzer->getName()).arg(
                    result->getPluginState().value("error").toString());
        emit reportError(errorString);
        emit finishedFail(m_id, errorString);
        return;
    }

    QSharedPointer<PluginAction> action =
        QSharedPointer<PluginAction>(
                new PluginAction(
                        PluginAction::Analyzer,
                        m_analyzer->getName(),
                        result->getPluginState()));
    PluginActionLineage::recordLineage(action, {m_container}, {m_container});

    if (!result->bitInfo().isNull()) {
        m_container->setBitInfo(result->bitInfo());
    }
}

QSharedPointer<const AnalyzerResult> AnalyzerRunner::analyzerCall(
        QSharedPointer<AnalyzerInterface> analyzer,
        QSharedPointer<const BitContainer> bits,
        QJsonObject pluginState,
        QSharedPointer<ActionProgress> progressTracker)
{
    return analyzer->analyzeBits(bits, pluginState, progressTracker);
}
