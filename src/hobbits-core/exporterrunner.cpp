#include "exporterrunner.h"
#include "pluginaction.h"
#include "pluginactionlineage.h"
#include "pluginactionmanager.h"
#include "settingsmanager.h"
#include <QJsonObject>

ExporterRunner::ExporterRunner(QString pluginName, QString pluginFileLocation) :
    AbstractPluginRunner<ExportResult>(pluginName, pluginFileLocation)
{

}

QSharedPointer<ExporterRunner> ExporterRunner::create(QSharedPointer<const HobbitsPluginManager> pluginManager, QSharedPointer<const PluginAction> action)
{
    if (action->pluginType() != PluginAction::Exporter) {
        return nullptr;
    }
    auto plugin = pluginManager->getImporterExporter(action->pluginName());
    if (!plugin || !plugin->canExport()) {
        return nullptr;
    }

    auto runner = QSharedPointer<ExporterRunner>(new ExporterRunner(plugin->name(), pluginManager->getPluginLocation(plugin->name())));

    runner->m_exporter = plugin;
    runner->m_action = action;

    return runner;
}

QSharedPointer<PluginActionWatcher<QSharedPointer<ExportResult> > > ExporterRunner::run(QSharedPointer<BitContainer> container)
{
    auto parameters = m_action->parameters();
    if (!commonPreRun(parameters)) {
        return QSharedPointer<PluginActionWatcher<QSharedPointer<ExportResult>>>();
    }

    QSharedPointer<PluginActionProgress> progress(new PluginActionProgress());
    auto future = QtConcurrent::run(
            QThreadPool::globalInstance(),
            ExporterRunner::exporterCall,
            m_exporter,
            container,
            parameters,
            progress);

    return commonRunSetup(future, progress);
}

void ExporterRunner::postProcess()
{
    if (!commonPostRun()) {
        return;
    }

    // TODO: some kind of lineage recording for exports so that they can be saved in batches (?)

    emit finished(m_id);
}


QSharedPointer<ExportResult> ExporterRunner::exporterCall(
        QSharedPointer<ImporterExporterInterface> exporter,
        QSharedPointer<const BitContainer> container,
        QJsonObject parameters,
        QSharedPointer<PluginActionProgress> progressTracker)
{
    try {
        return exporter->exportBits(container, parameters, progressTracker);
    } catch (std::exception &e) {
        return ExportResult::error(QString("Exception encountered in plugin %1: %2").arg(exporter->name()).arg(e.what()));
    } catch (...) {
        return ExportResult::error(QString("Unexpected exception in plugin %1").arg(exporter->name()));
    }
}
