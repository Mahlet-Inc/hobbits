#ifndef PLUGINACTIONMANAGER_H
#define PLUGINACTIONMANAGER_H

#include <QObject>
#include <QQueue>
#include <QStack>
#include <QHash>

#include "actionwatcher.h"
#include "analyzerrunner.h"
#include "operatorrunner.h"
#include "hobbitspluginmanager.h"
#include <QSharedPointer>
#include "pluginactionbatch.h"
#include "batchrunner.h"

#include "hobbits-core_global.h"

class HOBBITSCORESHARED_EXPORT PluginActionManager : public QObject, public QEnableSharedFromThis<PluginActionManager>
{
    Q_OBJECT

public:
    PluginActionManager(QSharedPointer<const HobbitsPluginManager> pluginManager);

    void setContainerManager(QSharedPointer<BitContainerManager> containerManager);

    QSharedPointer<AnalyzerRunner> runAnalyzer(QSharedPointer<const PluginAction> action, QSharedPointer<BitContainer> container);
    QSharedPointer<OperatorRunner> runOperator(
            QSharedPointer<const PluginAction> action,
            QList<QSharedPointer<BitContainer>> containers);
    QSharedPointer<ImportResult> runImporter(QSharedPointer<const PluginAction> action);
    QSharedPointer<ExportResult> runExporter(QSharedPointer<const PluginAction> action, QSharedPointer<BitContainer> container);

    const QHash<QUuid, QSharedPointer<AnalyzerRunner>> runningAnalyzers() const;
    const QHash<QUuid, QSharedPointer<OperatorRunner>> runningOperators() const;
    const QHash<QUuid, QSharedPointer<BatchRunner>> runningBatches() const;

    void runBatch(QSharedPointer<PluginActionBatch> batch, QList<QSharedPointer<BitContainer>> containers);

private slots:
    void finishOperator(QUuid);
    void finishAnalyzer(QUuid);
    void finishBatch(QUuid);

    void relayErrorFromOperator(QUuid, QString);
    void relayErrorFromAnalyzer(QUuid, QString);


public slots:
    void cancelById(QUuid);

signals:
    void analyzerStarted(QUuid);
    void analyzerProgress(QUuid, int);
    void analyzerFinished(QUuid);

    void operatorStarted(QUuid);
    void operatorProgress(QUuid, int);
    void operatorFinished(QUuid);

    void batchFinished(QUuid);

    void reportError(QString);

private:

    QSharedPointer<const HobbitsPluginManager> m_pluginManager;
    QSharedPointer<BitContainerManager> m_containerManager;

    QHash<QUuid, QSharedPointer<BatchRunner>> m_batchRunners;

    QHash<QUuid, QSharedPointer<OperatorRunner>> m_operatorRunners;
    QHash<QUuid, QSharedPointer<AnalyzerRunner>> m_analyzerRunners;

};

#endif // PLUGINACTIONMANAGER_H
