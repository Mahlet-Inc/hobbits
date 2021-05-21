#include "analyzerrunner.h"
#include "pluginaction.h"
#include "pluginactionlineage.h"
#include "pluginactionmanager.h"
#include "settingsmanager.h"

AnalyzerRunner::AnalyzerRunner(QString pluginName, QString pluginFileLocation) :
    AbstractPluginRunner(pluginName, pluginFileLocation)
{

}

QSharedPointer<AnalyzerRunner> AnalyzerRunner::create(
        QSharedPointer<const HobbitsPluginManager> pluginManager,
        QSharedPointer<const PluginAction> action)
{
    if (action->pluginType() != PluginAction::Analyzer) {
        return nullptr;
    }
    auto plugin = pluginManager->getAnalyzer(action->pluginName());
    if (!plugin) {
        return nullptr;
    }

    auto runner = QSharedPointer<AnalyzerRunner>(new AnalyzerRunner(plugin->name(), pluginManager->getPluginLocation(plugin->name())));

    runner->m_analyzer = plugin;
    runner->m_action = action;

    return runner;
}

QSharedPointer<BitContainer> AnalyzerRunner::container() const
{
    return m_container;
}

QSharedPointer<PluginActionWatcher<QSharedPointer<const AnalyzerResult>>> AnalyzerRunner::run(QSharedPointer<BitContainer> container)
{
    auto parameters = m_action->parameters();
    if (!commonPreRun(parameters)) {
        return QSharedPointer<PluginActionWatcher<QSharedPointer<const AnalyzerResult>>>();
    }

    QSharedPointer<PluginActionProgress> progress(new PluginActionProgress());

    auto future = QtConcurrent::run(
            QThreadPool::globalInstance(),
            AnalyzerRunner::analyzerCall,
            m_analyzer,
            container,
            parameters,
            progress);

    m_container = container;

    return commonRunSetup(future, progress);
}

void AnalyzerRunner::postProcess()
{
    if (!commonPostRun()) {
        return;
    }

    QSharedPointer<PluginAction> action =
        QSharedPointer<PluginAction>(
                new PluginAction(
                        PluginAction::Analyzer,
                        m_analyzer->name(),
                        m_result->parameters()));
    PluginActionLineage::recordLineage(action, {m_container}, {m_container});

    if (!m_result->bitInfo().isNull()) {
        m_container->setInfo(m_result->bitInfo());
    }

    emit finished(m_id);
}

QSharedPointer<const AnalyzerResult> AnalyzerRunner::analyzerCall(
        QSharedPointer<AnalyzerInterface> analyzer,
        QSharedPointer<const BitContainer> bits,
        const Parameters &parameters,
        QSharedPointer<PluginActionProgress> progressTracker)
{
    try {
        return analyzer->analyzeBits(bits, parameters, progressTracker);
    } catch (std::exception &e) {
        return AnalyzerResult::error(QString("Exception encountered in plugin %1: %2").arg(analyzer->name()).arg(e.what()));
    } catch (...) {
        return AnalyzerResult::error(QString("Unexpected exception in plugin %1").arg(analyzer->name()));
    }
}
