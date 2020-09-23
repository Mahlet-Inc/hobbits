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
    auto plugin = m_pluginManager->getAnalyzer(action->getPluginName());
    if (!plugin) {
        reportError(QString("Analyzer plugin named '%1' could not be loaded.").arg(action->getPluginName()));
        QPair<QUuid, QSharedPointer<AnalyzerRunner>>();
    }

    auto runner = AnalyzerRunner::create(m_pluginManager, action);

    m_analyzerRunners.insert(runner->id(), runner);
    connect(runner.data(), &AnalyzerRunner::reportError, this, &PluginActionManager::relayErrorFromAnalyzer);
    connect(runner.data(), &AnalyzerRunner::finished, this, &PluginActionManager::finishAnalyzer);
    connect(runner.data(), SIGNAL(progress(QUuid, int)), this, SIGNAL(analyzerProgress(QUuid, int)));

    runner->run(container);
    emit analyzerStarted(runner->id());

    return runner;
}

QSharedPointer<OperatorRunner> PluginActionManager::runOperator(
        QSharedPointer<const PluginAction> action,
        QList<QSharedPointer<BitContainer>> containers)
{
    auto plugin = m_pluginManager->getOperator(action->getPluginName());
    if (!plugin) {
        reportError(QString("Operator plugin named '%1' could not be loaded.").arg(action->getPluginName()));
        QPair<QUuid, QSharedPointer<AnalyzerRunner>>();
    }

    auto runner = OperatorRunner::create(m_pluginManager, m_containerManager, action);

    m_operatorRunners.insert(runner->id(), runner);
    connect(runner.data(), &OperatorRunner::reportError, this, &PluginActionManager::relayErrorFromOperator);
    connect(runner.data(), &OperatorRunner::finished, this, &PluginActionManager::finishOperator);
    connect(runner.data(), SIGNAL(progress(QUuid, int)), this, SIGNAL(operatorProgress(QUuid, int)));

    runner->run(containers);
    emit operatorStarted(runner->id());

    return runner;
}

QSharedPointer<ImportResult> PluginActionManager::runImporter(QSharedPointer<const PluginAction> action)
{
    auto plugin = m_pluginManager->getImporterExporter(action->getPluginName());
    if (!plugin) {
        reportError(QString("Importer plugin named '%1' could not be loaded.").arg(action->getPluginName()));
        return QSharedPointer<ImportResult>();
    }

    QSharedPointer<ImportResult> result;
    try {
        result = plugin->importBits(action->getPluginState());
    } catch (std::exception &e) {
        result = ImportResult::error(QString("Exception encountered in plugin %1: %2").arg(plugin->getName()).arg(e.what()));
    } catch (...) {
        result = ImportResult::error(QString("Unexpected exception in plugin %1").arg(plugin->getName()));
    }
    if (!result->errorString().isEmpty()) {
        reportError(QString("'%1' Error: %2").arg(action->getPluginName()).arg(result->errorString()));
    }
    else if (!result->hasEmptyState() && !result->getContainer().isNull()) {
        auto fullAction = PluginAction::importerAction(action->getPluginName(), result->pluginState());
        PluginActionLineage::recordLineage(fullAction, {}, {result->getContainer()});
    }

    if (!result->getContainer().isNull() && !m_containerManager.isNull()) {
        if (m_containerManager->addContainer(result->getContainer())) {
            m_containerManager->selectContainer(result->getContainer());
        }
    }

    return result;
}

QSharedPointer<ExportResult> PluginActionManager::runExporter(QSharedPointer<const PluginAction> action, QSharedPointer<BitContainer> container)
{
    auto plugin = m_pluginManager->getImporterExporter(action->getPluginName());
    if (!plugin) {
        reportError(QString("Exporter plugin named '%1' could not be loaded.").arg(action->getPluginName()));
        return QSharedPointer<ExportResult>();
    }

    QSharedPointer<ExportResult> result;
    try {
        result = plugin->exportBits(container, action->getPluginState());
    } catch (std::exception &e) {
        result = ExportResult::error(QString("Exception encountered in plugin %1: %2").arg(plugin->getName()).arg(e.what()));
    } catch (...) {
        result = ExportResult::error(QString("Unexpected exception in plugin %1").arg(plugin->getName()));
    }
    if (!result->errorString().isEmpty()) {
        reportError(QString("'%1' Error: %2").arg(action->getPluginName()).arg(result->errorString()));
    }

    return result;
}

void PluginActionManager::cancelById(QUuid id)
{
    if (m_analyzerRunners.contains(id)) {
        m_analyzerRunners.value(id)->getWatcher()->progress()->setCancelled(true);
    }
    else if (m_operatorRunners.contains(id)) {
        m_operatorRunners.value(id)->getWatcher()->progress()->setCancelled(true);
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

void PluginActionManager::runBatch(QSharedPointer<PluginActionBatch> batch, QList<QSharedPointer<BitContainer>> containers)
{
    while (containers.size() < batch->getMinRequiredInputs(m_pluginManager)) {
        auto result = runImporter(PluginAction::importerAction("File Data"));
        if (result.isNull() || result->hasEmptyState()) {
            emit reportError(QString("Cannot execute batch without %1 additional imported containers").arg(batch->getMinRequiredInputs(m_pluginManager) - containers.size()));
            return;
        }
        if (!result->errorString().isEmpty()) {
            emit reportError(result->errorString());
            return;
        }
    }

    auto runner = BatchRunner::create(batch, containers);
    m_batchRunners.insert(runner->id(), runner);
    connect(runner.data(), &BatchRunner::finished, this, &PluginActionManager::finishBatch);
    runner->run(sharedFromThis());
}
