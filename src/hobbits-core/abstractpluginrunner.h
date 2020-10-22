#ifndef ABSTRACTPLUGINRUNNER_H
#define ABSTRACTPLUGINRUNNER_H

#include <QObject>
#include <QUuid>
#include "pluginactionwatcher.h"
#include "hobbits-core_global.h"

class HOBBITSCORESHARED_EXPORT AbstractPluginRunnerQObject : public QObject {
    Q_OBJECT

signals:
    void reportError(QUuid, QString);
    void progress(QUuid, int);
    void finished(QUuid);

protected slots:
    virtual void postProcess() = 0;
};

template <class T>
class Q_DECL_EXPORT AbstractPluginRunner : public AbstractPluginRunnerQObject
{
public:
    AbstractPluginRunner(QString pluginName, QString pluginFileLocation);

    QUuid id() const;
    QString pluginName() const;
    QSharedPointer<T> result();
    QSharedPointer<PluginActionWatcher<QSharedPointer<T>>> watcher();


protected:
    bool commonPreRun(const QJsonObject &parameters);
    QSharedPointer<PluginActionWatcher<QSharedPointer<T>>> commonRunSetup(
            QFuture<QSharedPointer<T>> future,
            QSharedPointer<PluginActionProgress> progress);
    bool commonPostRun();

    QUuid m_id;
    QString m_pluginName;
    QString m_pluginFileLocation;

    QSharedPointer<T> m_result;
    QSharedPointer<PluginActionWatcher<QSharedPointer<T>>> m_actionWatcher;
};

#include "abstractpluginrunner.cpp"

#endif // ABSTRACTPLUGINRUNNER_H
