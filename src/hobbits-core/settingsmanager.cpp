#include "settingsmanager.h"
#include <QDir>

SettingsManager::SettingsManager() :
    m_hasRead(false)
{

}

SettingsManager& SettingsManager::getInstance()
{
    static SettingsManager instance;
    return instance;
}

void SettingsManager::setConfigFilePath(const QString &configFilePath)
{
    m_configFilePath = configFilePath;
}

void SettingsManager::writeSettings()
{
    if (m_configFilePath.isEmpty()) {
        QSettings settings("Hobbits", "Hobbits GUI");
        writeToSettings(settings);
    }
    else {
        QSettings settings(m_configFilePath, QSettings::IniFormat);
        writeToSettings(settings);
    }
}

void SettingsManager::readSettings()
{
    if (m_hasRead) {
        return;
    }

    if (m_configFilePath.isEmpty()) {
        QSettings settings("Hobbits", "Hobbits GUI");
        readFromSettings(settings);
    }
    else {
        QSettings settings(m_configFilePath, QSettings::IniFormat);
        readFromSettings(settings);
    }
}

void SettingsManager::writeToSettings(QSettings &settings)
{
    settings.beginGroup("UI");
    for (QString key : m_data.getUiSettingKeys()) {
        settings.setValue(key, m_data.getUiSetting(key));
    }
    settings.endGroup();

    settings.beginGroup("Plugin Loader");
    for (QString key : m_data.getPluginLoaderSettingKeys()) {
        settings.setValue(key, m_data.getPluginLoaderSetting(key));
    }
    settings.endGroup();

    settings.beginGroup("Plugin Settings");
    for (QString key : m_data.getPluginSettingKeys()) {
        settings.setValue(key, m_data.getPluginSetting(key));
    }
    settings.endGroup();

    settings.beginGroup("Private");
    for (QString key : m_data.getPrivateSettingKeys()) {
        settings.setValue(key, m_data.getPrivateSetting(key));
    }
    settings.endGroup();
}

void SettingsManager::readFromSettings(QSettings &settings)
{
    settings.beginGroup("UI");
    for (QString key : settings.allKeys()) {
        m_data.setUiSetting(key, settings.value(key));
    }
    settings.endGroup();

    settings.beginGroup("Plugin Loader");
    for (QString key : settings.allKeys()) {
        m_data.setPluginLoaderSetting(key, settings.value(key));
    }
    settings.endGroup();

    settings.beginGroup("Plugin Settings");
    for (QString key : settings.allKeys()) {
        m_data.setPluginSetting(key, settings.value(key));
    }
    settings.endGroup();

    settings.beginGroup("Private");
    for (QString key : settings.allKeys()) {
        m_data.setPrivateSetting(key, settings.value(key));
    }
    settings.endGroup();
}

SettingsData SettingsManager::getAll() const
{
    return m_data;
}

void SettingsManager::setAll(const SettingsData &data)
{
    m_data = data;
    writeSettings();
}

QVariant SettingsManager::getPrivateSetting(const QString &key)
{
    readSettings();
    return m_data.getPrivateSetting(key);

}

void SettingsManager::setPrivateSetting(const QString &key, const QVariant &value)
{
    m_data.setPrivateSetting(key, value);
    writeSettings();
}

QVariant SettingsManager::getUiSetting(const QString &key)
{
    readSettings();
    return m_data.getUiSetting(key);

}

void SettingsManager::setUiSetting(const QString &key, const QVariant &value)
{
    m_data.setUiSetting(key, value);
    writeSettings();
}

QVariant SettingsManager::getPluginLoaderSetting(const QString &key)
{
    readSettings();
    return m_data.getPluginLoaderSetting(key);

}

void SettingsManager::setPluginLoaderSetting(const QString &key, const QVariant &value)
{
    m_data.setPluginLoaderSetting(key, value);
    writeSettings();
}

QVariant SettingsManager::getPluginSetting(const QString &key)
{
    readSettings();
    return m_data.getPluginSetting(key);
}

void SettingsManager::setPluginSetting(const QString &key, const QVariant &value)
{
    m_data.setPluginSetting(key, value);
    writeSettings();
}
