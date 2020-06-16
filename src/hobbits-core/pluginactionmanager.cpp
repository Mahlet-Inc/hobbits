#include "pluginactionmanager.h"
#include "settingsmanager.h"

PluginActionManager::PluginActionManager(QSharedPointer<const HobbitsPluginManager> pluginManager) :
    QObject(nullptr),
    m_pluginManager(pluginManager)
{
}


QSharedPointer<AnalyzerRunner> PluginActionManager::runAnalyzer(QSharedPointer<PluginAction> action, QSharedPointer<BitContainer> container)
{
    auto plugin = m_pluginManager->getAnalyzer(action->getPluginName());
    if (!plugin) {
        reportError(QString("Analyzer plugin named '%1' could not be loaded.").arg(action->getPluginName()));
        QPair<QUuid, QSharedPointer<AnalyzerRunner>>();
    }

    auto runner = AnalyzerRunner::create(m_pluginManager, action);

    m_operatorRunners.insert(runner->id(), runner);
    connect(runner.data(), &AnalyzerRunner::reportError, this, &PluginActionManager::relayErrorFromAnalyzer);
    connect(runner.data(), &AnalyzerRunner::finished, this, &PluginActionManager::finishAnalyzer);

    runner->run(container);

    return runner;
}

QSharedPointer<OperatorRunner> PluginActionManager::runOperator(QSharedPointer<PluginAction> action, QList<QSharedPointer<BitContainer>> containers)
{
    auto plugin = m_pluginManager->getOperator(action->getPluginName());
    if (!plugin) {
        reportError(QString("Operator plugin named '%1' could not be loaded.").arg(action->getPluginName()));
        QPair<QUuid, QSharedPointer<AnalyzerRunner>>();
    }

    auto runner = OperatorRunner::create(m_pluginManager, action);

    m_operatorRunners.insert(runner->id(), runner);
    connect(runner.data(), &OperatorRunner::reportError, this, &PluginActionManager::relayErrorFromOperator);
    connect(runner.data(), &OperatorRunner::finished, this, &PluginActionManager::finishOperator);

    runner->run(containers);

    return runner;
}

QSharedPointer<ImportResult> PluginActionManager::runImporter(QSharedPointer<PluginAction> action)
{
    auto plugin = m_pluginManager->getImporterExporter(action->getPluginName());
    if (!plugin) {
        reportError(QString("Importer plugin named '%1' could not be loaded.").arg(action->getPluginName()));
        return QSharedPointer<ExportResult>();
    }

    auto result = plugin->importBits(action->getPluginState());
    if (!result->errorString().isEmpty()) {
        reportError(QString("'%1' Error: %2").arg(action->getPluginName()).arg(result->errorString()));
    }

    return result;
}

QSharedPointer<ExportResult> PluginActionManager::runExporter(QSharedPointer<PluginAction> action, QSharedPointer<BitContainer> container)
{
    auto plugin = m_pluginManager->getImporterExporter(action->getPluginName());
    if (!plugin) {
        reportError(QString("Exporter plugin named '%1' could not be loaded.").arg(action->getPluginName()));
        return QSharedPointer<ExportResult>();
    }

    auto result = plugin->exportBits(container, action->getPluginState());
    if (!result->errorString().isEmpty()) {
        reportError(QString("'%1' Error: %2").arg(action->getPluginName()).arg(result->errorString()));
    }

    return result;
}

const QHash<QUuid, QSharedPointer<AnalyzerRunner>> PluginActionManager::runningAnalyzers() const
{
    return m_analyzerRunners;
}

const QHash<QUuid, QSharedPointer<OperatorRunner>> PluginActionManager::runningOperators() const
{
    return m_operatorRunners;
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

    m_batches.insert(QUuid::createUuid(), batch);
}
