#include "abstractpluginrunner.h"
#include "settingsmanager.h"
#include <QJsonObject>

template <class T>
AbstractPluginRunner<T>::AbstractPluginRunner(QString pluginName, QString pluginFileLocation) :
    m_pluginName(pluginName),
    m_pluginFileLocation(pluginFileLocation)
{
    m_id = QUuid::createUuid();
}

template <class T>
QUuid AbstractPluginRunner<T>::id() const
{
    return m_id;
}

template <class T>
QString AbstractPluginRunner<T>::pluginName() const
{
    return m_pluginName;
}

template<class T>
QSharedPointer<T> AbstractPluginRunner<T>::result()
{
    return m_result;
}

template <class T>
QSharedPointer<PluginActionWatcher<QSharedPointer<T>>> AbstractPluginRunner<T>::watcher()
{
    return m_actionWatcher;
}

template <class T>
bool AbstractPluginRunner<T>::commonPreRun(const QJsonObject &parameters)
{
    if (!m_actionWatcher.isNull() && m_actionWatcher->watcher()->future().isRunning()) {
        emit reportError(m_id, QString("Runner is already running"));
        return false;
    }

    if (parameters.isEmpty()) {
        emit reportError(m_id, QString("Cannot run plugin '%1' without parameters").arg(m_pluginName));
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

template<class T>
QSharedPointer<PluginActionWatcher<QSharedPointer<T> > > AbstractPluginRunner<T>::commonRunSetup(QFuture<QSharedPointer<T>> future, QSharedPointer<PluginActionProgress> progress)
{
    m_actionWatcher = QSharedPointer<PluginActionWatcher<QSharedPointer<T>>>(
            new PluginActionWatcher<QSharedPointer<T>>(
                    future,
                    progress));

    connect(m_actionWatcher->watcher(), SIGNAL(finished()), this, SLOT(postProcess()));
    connect(m_actionWatcher->progress().data(), &PluginActionProgress::progressPercentChanged, [this](int prog) {
        this->progress(m_id, prog);
    });

    return m_actionWatcher;
}

template <class T>
bool AbstractPluginRunner<T>::commonPostRun()
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
