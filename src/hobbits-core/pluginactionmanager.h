#ifndef PLUGINACTIONMANAGER_H
#define PLUGINACTIONMANAGER_H

#include <QObject>
#include <QQueue>
#include <QStack>
#include <QHash>

#include "pluginactionwatcher.h"
#include "analyzerrunner.h"
#include "operatorrunner.h"
#include "hobbitspluginmanager.h"
#include <QSharedPointer>
#include "pluginactionbatch.h"
#include "batchrunner.h"
#include "importerrunner.h"
#include "exporterrunner.h"

#include "hobbits-core_global.h"

/**
  * @brief The PluginActionManager class manages the execution of PluginAction specifications
  *
  * \see PluginAction AnalyzerRunner OperatorRunner BatchRunner
*/
class HOBBITSCORESHARED_EXPORT PluginActionManager : public QObject, public QEnableSharedFromThis<PluginActionManager>
{
    Q_OBJECT

public:
    PluginActionManager(QSharedPointer<const HobbitsPluginManager> pluginManager);

    void setContainerManager(QSharedPointer<BitContainerManager> containerManager);

    QSharedPointer<AnalyzerRunner> runAnalyzer(QSharedPointer<const PluginAction> action, QSharedPointer<BitContainer> container);
    QSharedPointer<OperatorRunner> runOperator(QSharedPointer<const PluginAction> action, QList<QSharedPointer<BitContainer>> containers);
    QSharedPointer<ImporterRunner> runImporter(QSharedPointer<const PluginAction> action);
    QSharedPointer<ExporterRunner> runExporter(QSharedPointer<const PluginAction> action, QSharedPointer<BitContainer> container);

    const QHash<QUuid, QSharedPointer<ImporterRunner>> runningImporters() const;
    const QHash<QUuid, QSharedPointer<ExporterRunner>> runningExporters() const;
    const QHash<QUuid, QSharedPointer<AnalyzerRunner>> runningAnalyzers() const;
    const QHash<QUuid, QSharedPointer<OperatorRunner>> runningOperators() const;
    const QHash<QUuid, QSharedPointer<BatchRunner>> runningBatches() const;

    void runBatch(QSharedPointer<PluginActionBatch> batch, QList<QSharedPointer<BitContainer>> containers);

private slots:
    void finishOperator(QUuid);
    void finishAnalyzer(QUuid);
    void finishImporter(QUuid);
    void finishExporter(QUuid);
    void finishBatch(QUuid);

    void relayErrorFromOperator(QUuid, QString);
    void relayErrorFromAnalyzer(QUuid, QString);
    void relayErrorFromImporter(QUuid, QString);
    void relayErrorFromExporter(QUuid, QString);


public slots:
    void cancelById(QUuid);

signals:
    void analyzerStarted(QUuid);
    void analyzerProgress(QUuid, int);
    void analyzerFinished(QUuid);

    void operatorStarted(QUuid);
    void operatorProgress(QUuid, int);
    void operatorFinished(QUuid);

    void importerStarted(QUuid);
    void importerProgress(QUuid, int);
    void importerFinished(QUuid);

    void exporterStarted(QUuid);
    void exporterProgress(QUuid, int);
    void exporterFinished(QUuid);

    void batchFinished(QUuid);

    void reportError(QString);

private:

    QSharedPointer<const HobbitsPluginManager> m_pluginManager;
    QSharedPointer<BitContainerManager> m_containerManager;

    QHash<QUuid, QSharedPointer<BatchRunner>> m_batchRunners;

    QHash<QUuid, QSharedPointer<OperatorRunner>> m_operatorRunners;
    QHash<QUuid, QSharedPointer<AnalyzerRunner>> m_analyzerRunners;
    QHash<QUuid, QSharedPointer<ImporterRunner>> m_importerRunners;
    QHash<QUuid, QSharedPointer<ExporterRunner>> m_exporterRunners;

};

#endif // PLUGINACTIONMANAGER_H
