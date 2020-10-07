#include "pluginactionmanager.h"
#include "settingsmanager.h"

PluginActionManager::PluginActionManager(QSharedPointer<const HobbitsPluginManager> pluginManager) :
    QObject(nullptr),
    m_pluginManager(pluginManager)
{
}

void PluginActionManager::setContainerManager(QSharedPointer<BitContainerManager> containerManager)
{
    m_containerManager = containerManager;
}

QSharedPointer<AnalyzerRunner> PluginActionManager::runAnalyzer(QSharedPointer<const PluginAction> action, QSharedPointer<BitContainer> container)
{
    auto plugin = m_pluginManager->getAnalyzer(action->pluginName());
    if (!plugin) {
        reportError(QString("Analyzer plugin named '%1' could not be loaded.").arg(action->pluginName()));
        return QSharedPointer<AnalyzerRunner>();
    }

    auto runner = AnalyzerRunner::create(m_pluginManager, action);

    m_analyzerRunners.insert(runner->id(), runner);
    connect(runner.data(), &AnalyzerRunner::reportError, this, &PluginActionManager::relayErrorFromAnalyzer);
    connect(runner.data(), &AnalyzerRunner::finished, this, &PluginActionManager::finishAnalyzer);
    connect(runner.data(), SIGNAL(progress(QUuid, int)), this, SIGNAL(analyzerProgress(QUuid, int)));

    auto watcher = runner->run(container);
    emit analyzerStarted(runner->id());
    if (watcher.isNull()) {
        finishAnalyzer(runner->id());
        return QSharedPointer<AnalyzerRunner>();
    }

    return runner;
}

QSharedPointer<OperatorRunner> PluginActionManager::runOperator(
        QSharedPointer<const PluginAction> action,
        QList<QSharedPointer<BitContainer>> containers)
{
    auto plugin = m_pluginManager->getOperator(action->pluginName());
    if (!plugin) {
        reportError(QString("Operator plugin named '%1' could not be loaded.").arg(action->pluginName()));
        return QSharedPointer<OperatorRunner>();
    }

    auto runner = OperatorRunner::create(m_pluginManager, m_containerManager, action);

    m_operatorRunners.insert(runner->id(), runner);
    connect(runner.data(), &OperatorRunner::reportError, this, &PluginActionManager::relayErrorFromOperator);
    connect(runner.data(), &OperatorRunner::finished, this, &PluginActionManager::finishOperator);
    connect(runner.data(), SIGNAL(progress(QUuid, int)), this, SIGNAL(operatorProgress(QUuid, int)));

    auto watcher = runner->run(containers);
    emit operatorStarted(runner->id());
    if (watcher.isNull()) {
        finishOperator(runner->id());
        return QSharedPointer<OperatorRunner>();
    }

    return runner;
}

QSharedPointer<ImporterRunner> PluginActionManager::runImporter(QSharedPointer<const PluginAction> action)
{
    auto plugin = m_pluginManager->getImporterExporter(action->pluginName());
    if (!plugin) {
        reportError(QString("Importer plugin named '%1' could not be loaded.").arg(action->pluginName()));
        return QSharedPointer<ImporterRunner>();
    }

    auto runner = ImporterRunner::create(m_pluginManager, m_containerManager, action);

    m_importerRunners.insert(runner->id(), runner);
    connect(runner.data(), &ImporterRunner::reportError, this, &PluginActionManager::relayErrorFromImporter);
    connect(runner.data(), &ImporterRunner::finished, this, &PluginActionManager::finishImporter);
    connect(runner.data(), SIGNAL(progress(QUuid, int)), this, SIGNAL(importerProgress(QUuid, int)));

    auto watcher = runner->run();
    emit importerStarted(runner->id());
    if (watcher.isNull()) {
        finishImporter(runner->id());
        return QSharedPointer<ImporterRunner>();
    }

    return runner;
}

QSharedPointer<ExporterRunner> PluginActionManager::runExporter(QSharedPointer<const PluginAction> action, QSharedPointer<BitContainer> container)
{
    auto plugin = m_pluginManager->getImporterExporter(action->pluginName());
    if (!plugin) {
        reportError(QString("Exporter plugin named '%1' could not be loaded.").arg(action->pluginName()));
        return QSharedPointer<ExporterRunner>();
    }

    auto runner = ExporterRunner::create(m_pluginManager, action);

    m_exporterRunners.insert(runner->id(), runner);
    connect(runner.data(), &ExporterRunner::reportError, this, &PluginActionManager::relayErrorFromExporter);
    connect(runner.data(), &ExporterRunner::finished, this, &PluginActionManager::finishExporter);
    connect(runner.data(), SIGNAL(progress(QUuid, int)), this, SIGNAL(exporterProgress(QUuid, int)));

    auto watcher = runner->run(container);
    emit exporterStarted(runner->id());
    if (watcher.isNull()) {
        finishExporter(runner->id());
        return QSharedPointer<ExporterRunner>();
    }

    return runner;
}

const QHash<QUuid, QSharedPointer<ImporterRunner> > PluginActionManager::runningImporters() const
{
    return m_importerRunners;
}

const QHash<QUuid, QSharedPointer<ExporterRunner> > PluginActionManager::runningExporters() const
{
    return m_exporterRunners;
}

void PluginActionManager::cancelById(QUuid id)
{
    if (m_analyzerRunners.contains(id)) {
        m_analyzerRunners.value(id)->watcher()->progress()->setCancelled(true);
    }
    else if (m_operatorRunners.contains(id)) {
        m_operatorRunners.value(id)->watcher()->progress()->setCancelled(true);
    }
    else if (m_importerRunners.contains(id)) {
        m_importerRunners.value(id)->watcher()->progress()->setCancelled(true);
    }
    else if (m_exporterRunners.contains(id)) {
        m_exporterRunners.value(id)->watcher()->progress()->setCancelled(true);
    }
    else if (m_batchRunners.contains(id)) {
        m_batchRunners.value(id)->cancel();
    }
}

const QHash<QUuid, QSharedPointer<AnalyzerRunner>> PluginActionManager::runningAnalyzers() const
{
    return m_analyzerRunners;
}

const QHash<QUuid, QSharedPointer<OperatorRunner>> PluginActionManager::runningOperators() const
{
    return m_operatorRunners;
}

const QHash<QUuid, QSharedPointer<BatchRunner>> PluginActionManager::runningBatches() const
{
    return m_batchRunners;
}

void PluginActionManager::finishOperator(QUuid id)
{
    auto runner = m_operatorRunners.take(id);
    if (!runner.isNull()) {
        disconnect(runner.data(), &OperatorRunner::reportError, this, &PluginActionManager::relayErrorFromOperator);
        disconnect(runner.data(), &OperatorRunner::finished, this, &PluginActionManager::operatorFinished);
    }
    emit operatorFinished(id);
}

void PluginActionManager::finishAnalyzer(QUuid id)
{
    auto runner = m_analyzerRunners.take(id);
    if (!runner.isNull()) {
        disconnect(runner.data(), &AnalyzerRunner::reportError, this, &PluginActionManager::relayErrorFromAnalyzer);
        disconnect(runner.data(), &AnalyzerRunner::finished, this, &PluginActionManager::finishAnalyzer);
    }
    emit analyzerFinished(id);
}

void PluginActionManager::finishImporter(QUuid id)
{
    auto runner = m_importerRunners.take(id);
    if (!runner.isNull()) {
        disconnect(runner.data(), &ImporterRunner::reportError, this, &PluginActionManager::relayErrorFromImporter);
        disconnect(runner.data(), &ImporterRunner::finished, this, &PluginActionManager::finishImporter);
    }
    emit importerFinished(id);
}

void PluginActionManager::finishExporter(QUuid id)
{
    auto runner = m_exporterRunners.take(id);
    if (!runner.isNull()) {
        disconnect(runner.data(), &ExporterRunner::reportError, this, &PluginActionManager::relayErrorFromExporter);
        disconnect(runner.data(), &ExporterRunner::finished, this, &PluginActionManager::finishExporter);
    }
    emit exporterFinished(id);
}

void PluginActionManager::finishBatch(QUuid id)
{
    auto runner = m_batchRunners.take(id);
    if (!runner.isNull()) {
        disconnect(runner.data(), &BatchRunner::finished, this, &PluginActionManager::finishBatch);
        if (!runner->errorList().isEmpty()) {
            emit reportError("Batch Process Errors:\n" + runner->errorList().join("\n"));
        }
    }
    emit batchFinished(id);
}

void PluginActionManager::relayErrorFromOperator(QUuid id, QString errorString)
{
    // TODO: runners should have a human-referenceable name that can be included in the error report
    Q_UNUSED(id)
    emit reportError(QString("Operator Plugin Error: %1").arg(errorString));
}

void PluginActionManager::relayErrorFromAnalyzer(QUuid id, QString errorString)
{
    // TODO: runners should have a human-referenceable name that can be included in the error report
    Q_UNUSED(id)
    emit reportError(QString("Analyzer Plugin Error: %1").arg(errorString));
}

void PluginActionManager::relayErrorFromImporter(QUuid id, QString errorString)
{
    // TODO: runners should have a human-referenceable name that can be included in the error report
    Q_UNUSED(id)
    emit reportError(QString("Importer Plugin Error: %1").arg(errorString));
}

void PluginActionManager::relayErrorFromExporter(QUuid id, QString errorString)
{
    // TODO: runners should have a human-referenceable name that can be included in the error report
    Q_UNUSED(id)
    emit reportError(QString("Exporter Plugin Error: %1").arg(errorString));
}

void PluginActionManager::runBatch(QSharedPointer<PluginActionBatch> batch, QList<QSharedPointer<BitContainer>> containers)
{
    // TODO: prompt for batches with incomplete input containers
//    while (containers.size() < batch->getMinRequiredInputs(m_pluginManager)) {
//        auto result = runImporter(PluginAction::importerAction("File Data"));
//        if (result.isNull() || result->hasEmptyParameters()) {
//            emit reportError(QString("Cannot execute batch without %1 additional imported containers").arg(batch->getMinRequiredInputs(m_pluginManager) - containers.size()));
//            return;
//        }
//        if (!result->errorString().isEmpty()) {
//            emit reportError(result->errorString());
//            return;
//        }
//    }

    auto runner = BatchRunner::create(batch, containers);
    m_batchRunners.insert(runner->id(), runner);
    connect(runner.data(), &BatchRunner::finished, this, &PluginActionManager::finishBatch);
    runner->run(sharedFromThis());
}
