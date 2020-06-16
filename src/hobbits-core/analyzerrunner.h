#ifndef ANALYZERRUNNER_H
#define ANALYZERRUNNER_H

#include "actionwatcher.h"
#include "analyzerresult.h"
#include "hobbitspluginmanager.h"
#include <QJsonObject>
#include <QObject>
#include <QtConcurrent/QtConcurrentRun>

#include "hobbits-core_global.h"

class HOBBITSCORESHARED_EXPORT AnalyzerRunner : public QObject
{
    Q_OBJECT

public:
    static QSharedPointer<AnalyzerRunner> create(
            QSharedPointer<const HobbitsPluginManager> pluginManager,
            QSharedPointer<PluginAction> action);


    QUuid id() const;
    QSharedPointer<ActionWatcher<QSharedPointer<const AnalyzerResult>>> getWatcher();

    QSharedPointer<ActionWatcher<QSharedPointer<const AnalyzerResult>>> run(QSharedPointer<BitContainer> container);

signals:
    void reportError(QUuid, QString);
    void finished(QUuid);

private slots:
    void postProcess();

private:
    static QSharedPointer<const AnalyzerResult> analyzerCall(
            QSharedPointer<AnalyzerInterface> analyzer,
            QSharedPointer<const BitContainer> bits,
            QJsonObject pluginState,
            QSharedPointer<ActionProgress> progressTracker);

    QUuid m_id;
    QSharedPointer<PluginAction> m_action;
    QSharedPointer<AnalyzerInterface> m_analyzer;
    QString m_pluginFileLocation;
    QSharedPointer<BitContainer> m_container;

    QSharedPointer<ActionWatcher<QSharedPointer<const AnalyzerResult>>> m_actionWatcher;
};

#endif // ANALYZERRUNNER_H
