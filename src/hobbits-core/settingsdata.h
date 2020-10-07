#ifndef SETTINGSDATA_H
#define SETTINGSDATA_H

#include <QMap>
#include <QVariant>
#include <QMutex>
#include "hobbits-core_global.h"

/**
  * @brief The SettingsData class provides a thread-safe, in-memory cache for hobbits settings values
  *
  * \see SettingsManager
*/
class HOBBITSCORESHARED_EXPORT SettingsData
{
public:
    SettingsData();
    SettingsData(const SettingsData &other);

    SettingsData& operator=(const SettingsData&);

    QVariant getTransientSetting(const QString &key, const QVariant &defaultValue = QVariant());
    void setTransientSetting(const QString &key, const QVariant &value);

    QList<QString> getPrivateSettingKeys() const;
    QVariant getPrivateSetting(const QString &key, const QVariant &defaultValue = QVariant());
    void setPrivateSetting(const QString &key, const QVariant &value);

    QList<QString> getUiSettingKeys() const;
    QVariant getUiSetting(const QString &key, const QVariant &defaultValue = QVariant());
    void setUiSetting(const QString &key, const QVariant &value);

    QList<QString> getPluginLoaderSettingKeys() const;
    QVariant getPluginLoaderSetting(const QString &key, const QVariant &defaultValue = QVariant());
    void setPluginLoaderSetting(const QString &key, const QVariant &value);

    QList<QString> getPluginSettingKeys() const;
    QVariant getPluginSetting(const QString &key, const QVariant &defaultValue = QVariant());
    void setPluginSetting(const QString &key, const QVariant &value);

private:
    QMap<QString, QVariant> m_transientSettings;
    QMap<QString, QVariant> m_privateSettings;
    QMap<QString, QVariant> m_uiSettings;
    QMap<QString, QVariant> m_pluginLoaderSettings;
    QMap<QString, QVariant> m_pluginSettings;
    QMutex m_mutex;
};

#endif // SETTINGSDATA_H
