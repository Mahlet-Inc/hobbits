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

#include "hobbits-core_global.h"

class HOBBITSCORESHARED_EXPORT PluginActionManager : public QObject
{
    Q_OBJECT

public:
    PluginActionManager(QSharedPointer<const HobbitsPluginManager> pluginManager);

    struct LineageAction {
        QUuid containerId;
        QList<QSharedPointer<const PluginActionLineage>> lineage;
        QList<QSharedPointer<BitContainer>> inputs;
        QList<QSharedPointer<BitContainer>> additionalInputs;
        QSharedPointer<BitContainerManager> bitContainerManager;
        QString baseName;
        QMap<int, QUuid> outputIdOverride;
        QList<QUuid> additionalInputIds;
        int step;
        int additionalStep;
    };

    QSharedPointer<AnalyzerRunner> runAnalyzer(QSharedPointer<PluginAction> action, QSharedPointer<BitContainer> container);
    QSharedPointer<OperatorRunner> runOperator(QSharedPointer<PluginAction> action, QList<QSharedPointer<BitContainer>> containers);
    QSharedPointer<ImportResult> runImporter(QSharedPointer<PluginAction> action);
    QSharedPointer<ExportResult> runExporter(QSharedPointer<PluginAction> action, QSharedPointer<BitContainer> container);

    const QHash<QUuid, QSharedPointer<AnalyzerRunner>> runningAnalyzers() const;
    const QHash<QUuid, QSharedPointer<OperatorRunner>> runningOperators() const;

    void runBatch(QSharedPointer<PluginActionBatch> batch, QList<QSharedPointer<BitContainer>> containers);

private slots:
    void finishOperator(QUuid);
    void finishAnalyzer(QUuid);

    void relayErrorFromOperator(QUuid, QString);
    void relayErrorFromAnalyzer(QUuid, QString);


public slots:

signals:
    void analyzerStarted(QUuid);
    void analyzerProgress(QUuid, int);
    void analyzerFinished(QUuid);

    void operatorStarted(QUuid);
    void operatorProgress(QUuid, int);
    void operatorFinished(QUuid);

    void reportError(QString);

private:

    QSharedPointer<const HobbitsPluginManager> m_pluginManager;

    QHash<QUuid, QSharedPointer<PluginActionBatch>> m_batches;

    QHash<QUuid, QSharedPointer<OperatorRunner>> m_operatorRunners;
    QHash<QUuid, QSharedPointer<AnalyzerRunner>> m_analyzerRunners;

};

#endif // PLUGINACTIONMANAGER_H
