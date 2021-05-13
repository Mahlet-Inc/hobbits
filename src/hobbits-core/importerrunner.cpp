#include "importerrunner.h"
#include "pluginaction.h"
#include "pluginactionlineage.h"

ImporterRunner::ImporterRunner(QString pluginName, QString pluginFileLocation) :
    AbstractPluginRunner<ImportResult>(pluginName, pluginFileLocation)
{

}

QSharedPointer<ImporterRunner> ImporterRunner::create(QSharedPointer<const HobbitsPluginManager> pluginManager,
                                                      QSharedPointer<BitContainerManager> containerManager,
                                                      QSharedPointer<const PluginAction> action)
{
    if (action->pluginType() != PluginAction::Importer) {
        return nullptr;
    }
    auto plugin = pluginManager->getImporterExporter(action->pluginName());
    if (!plugin) {
        return nullptr;
    }

    if (!plugin->canImport()) {
        return nullptr;
    }

    auto runner = QSharedPointer<ImporterRunner>(new ImporterRunner(plugin->name(), pluginManager->getPluginLocation(plugin->name())));

    runner->m_importer = plugin;
    runner->m_action = action;
    runner->m_containerManager = containerManager;

    return runner;
}

QSharedPointer<PluginActionWatcher<QSharedPointer<ImportResult>>> ImporterRunner::run()
{
    auto parameters = m_action->parameters();
    if (!commonPreRun(parameters)) {
        return QSharedPointer<PluginActionWatcher<QSharedPointer<ImportResult>>>();
    }

    QSharedPointer<PluginActionProgress> progress(new PluginActionProgress());

    auto future = QtConcurrent::run(
            QThreadPool::globalInstance(),
            ImporterRunner::importerCall,
            m_importer,
            parameters,
            progress);

    return commonRunSetup(future, progress);
}

void ImporterRunner::postProcess()
{
    if (!commonPostRun()) {
        return;
    }

    QSharedPointer<PluginAction> action =
        QSharedPointer<PluginAction>(
                new PluginAction(
                        PluginAction::Importer,
                        m_importer->name(),
                        m_result->parameters()));
    PluginActionLineage::recordLineage(action, {}, {m_result->getContainer()});

    auto container = m_result->getContainer();
    if (!container.isNull()) {
        m_containerManager->addContainer(container);
        m_containerManager->selectContainer(container);
    }

    emit finished(m_id);
}

QSharedPointer<ImportResult> ImporterRunner::importerCall(QSharedPointer<ImporterExporterInterface> importer, const Parameters &parameters, QSharedPointer<PluginActionProgress> progressTracker)
{
    try {
        return importer->importBits(parameters, progressTracker);
    } catch (std::exception &e) {
        return ImportResult::error(QString("Exception encountered in plugin %1: %2").arg(importer->name()).arg(e.what()));
    } catch (...) {
        return ImportResult::error(QString("Unexpected exception in plugin %1").arg(importer->name()));
    }
}
