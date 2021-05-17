#ifndef ABSTRACTPLUGINRUNNER_H
#define ABSTRACTPLUGINRUNNER_H

#include <QObject>
#include "parameters.h"
#include <QUuid>
#include "pluginactionwatcher.h"
#include "hobbits-core_global.h"
#include "settingsmanager.h"

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
    AbstractPluginRunner(QString pluginName, QString pluginFileLocation) :
        m_pluginName(pluginName),
        m_pluginFileLocation(pluginFileLocation)
    {
        m_id = QUuid::createUuid();
    }

    QUuid id() const
    {
        return m_id;
    }

    QString pluginName() const
    {
        return m_pluginName;
    }

    QSharedPointer<T> result()
    {
        return m_result;
    }

    QSharedPointer<PluginActionWatcher<QSharedPointer<T>>> watcher()
    {
        return m_actionWatcher;
    }

protected:
    bool commonPreRun(const Parameters &parameters)
    {
        if (!m_actionWatcher.isNull() && m_actionWatcher->watcher()->future().isRunning()) {
            emit reportError(m_id, QString("Runner is already running"));
            return false;
        }

        if (parameters.isNull()) {
            emit reportError(m_id, QString("Cannot run plugin '%1' with uninitialized parameters").arg(m_pluginName));
            return false;
        }

        QVariant previousRunning = SettingsManager::getPrivateSetting(SettingsManager::PLUGINS_RUNNING_KEY);
        QStringList runningPlugins;
        if (previousRunning.isValid() && previousRunning.canConvert<QStringList>()) {
            runningPlugins = previousRunning.toStringList();
        }
        runningPlugins.append(m_pluginFileLocation);
        SettingsManager::setPrivateSetting(SettingsManager::PLUGINS_RUNNING_KEY, QVariant(runningPlugins));

        return true;
    }

    QSharedPointer<PluginActionWatcher<QSharedPointer<T> > > commonRunSetup(QFuture<QSharedPointer<T>> future, QSharedPointer<PluginActionProgress> progress)
    {
        m_actionWatcher = QSharedPointer<PluginActionWatcher<QSharedPointer<T>>>(
                new PluginActionWatcher<QSharedPointer<T>>(
                        future,
                        progress,
                        true));

        connect(m_actionWatcher->watcher(), SIGNAL(finished()), this, SLOT(postProcess()));
        connect(m_actionWatcher->progress().data(), &PluginActionProgress::progressPercentChanged, [this](int prog) {
            this->progress(m_id, prog);
        });

        m_actionWatcher->setFutureInWatcher();

        return m_actionWatcher;
    }

    bool commonPostRun()
    {
        disconnect(m_actionWatcher->watcher(), SIGNAL(finished()), this, SLOT(postProcess()));
        disconnect(m_actionWatcher->progress().data(), &PluginActionProgress::progressPercentChanged, nullptr, nullptr);

        QVariant previousRunning = SettingsManager::getPrivateSetting(SettingsManager::PLUGINS_RUNNING_KEY);
        QStringList runningPlugins;
        if (previousRunning.isValid() && previousRunning.canConvert<QStringList>()) {
            runningPlugins = previousRunning.toStringList();
        }
        runningPlugins.removeOne(m_pluginFileLocation);
        SettingsManager::setPrivateSetting(SettingsManager::PLUGINS_RUNNING_KEY, QVariant(runningPlugins));

        m_result = m_actionWatcher->watcher()->future().result();
        if (m_result.isNull()) {
            QString errorString = QString("Plugin '%1' failed to execute.  Null result returned.").arg(m_pluginName);
            emit reportError(m_id, errorString);
            emit finished(m_id);
            return false;
        }

        if (!m_result->errorString().isEmpty()) {
            QString errorString = QString("Plugin '%1' reported an error with its processing: %2").arg(m_pluginName).arg(m_result->errorString());
            emit reportError(m_id, errorString);
            emit finished(m_id);
            return false;
        }

        return true;
    }

    QUuid m_id;
    QString m_pluginName;
    QString m_pluginFileLocation;

    QSharedPointer<T> m_result;
    QSharedPointer<PluginActionWatcher<QSharedPointer<T>>> m_actionWatcher;
};

#endif // ABSTRACTPLUGINRUNNER_H
