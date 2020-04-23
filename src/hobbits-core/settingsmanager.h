#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include "settingsdata.h"
#include <QSettings>
#include <QMutex>

#include "hobbits-core_global.h"

class HOBBITSCORESHARED_EXPORT SettingsManager
{
public:
    static SettingsManager& getInstance();

    void setConfigFilePath(const QString &configFilePath);

    void writeSettings();
    void readSettings();

    SettingsData getAll() const;
    void setAll(const SettingsData &data);

    QVariant getPrivateSetting(const QString &key);
    void setPrivateSetting(const QString &key, const QVariant &value);

    QVariant getUiSetting(const QString &key);
    void setUiSetting(const QString &key, const QVariant &value);

    QVariant getPluginLoaderSetting(const QString &key);
    void setPluginLoaderSetting(const QString &key, const QVariant &value);

    QVariant getPluginSetting(const QString &key);
    void setPluginSetting(const QString &key, const QVariant &value);

private:
    SettingsManager();

    void writeToSettings(QSettings &settings);
    void readFromSettings(QSettings &settings);

    bool m_hasRead;

    QString m_configFilePath;

    SettingsData m_data;
    QMutex m_mutex;

public:
    SettingsManager(SettingsManager const&) = delete;

    void operator=(SettingsManager const&) = delete;

};

#endif // SETTINGSMANAGER_H
