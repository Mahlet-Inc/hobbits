#include "settingsmanager.h"
#include "settingsdata.h"
#include "hobbitscoreconfig.h"
#include <QDir>
#include <QCoreApplication>

const QString SettingsManager::ONE_COLOR_KEY = "1-Bit Color";
const QString SettingsManager::ZERO_COLOR_KEY = "0-Bit Color";
const QString SettingsManager::BYTE_HUE_SAT_KEY = "Byte Hue and Saturation";
const QString SettingsManager::FOCUS_COLOR_KEY = "Range Focus Color";
const QString SettingsManager::HIGHLIGHT_1_COLOR_KEY = "Highlight 1";
const QString SettingsManager::HIGHLIGHT_2_COLOR_KEY = "Highlight 2";
const QString SettingsManager::HIGHLIGHT_3_COLOR_KEY = "Highlight 3";
const QString SettingsManager::HIGHLIGHT_4_COLOR_KEY = "Highlight 4";
const QString SettingsManager::HIGHLIGHT_5_COLOR_KEY = "Highlight 5";
const QString SettingsManager::PLUGIN_PATH_KEY = "Plugin Path";
const QString SettingsManager::PLUGIN_BLACKLIST_KEY = "Plugin Blacklist";
const QString SettingsManager::OPERATOR_DISPLAY_ORDER_KEY = "Operator Display Order";
const QString SettingsManager::ANALYZER_DISPLAY_ORDER_KEY = "Analyzer Display Order";
const QString SettingsManager::DISPLAY_DISPLAY_ORDER_KEY = "Display Display Order";
const QString SettingsManager::WINDOW_SIZE_KEY = "window_size";
const QString SettingsManager::WINDOW_POSITION_KEY = "window_position";
const QString SettingsManager::WINDOW_STATE_KEY = "window_state";
const QString SettingsManager::LAST_BATCH_PATH_KEY = "last_batch_path";
const QString SettingsManager::LAST_IMPORT_EXPORT_PATH_KEY = "last_import_export_path";
const QString SettingsManager::LAST_CONTAINER_PATH_KEY = "last_container_path";
const QString SettingsManager::PLUGINS_RUNNING_KEY = "plugins_running";
const QString SettingsManager::PYTHON_HOME_KEY = "python_home_dir";

SettingsManager::SettingsManager() :
    m_hasRead(false)
{

}

SettingsManager& SettingsManager::instance()
{
    static SettingsManager instance;
    return instance;
}

void SettingsManager::setConfigFilePath(const QString &configFilePath)
{
    QMutexLocker lock(&instance().m_mutex);
    instance().m_configFilePath = configFilePath;
}

SettingsData SettingsManager::getAll()
{
    return instance().m_data;
}

void SettingsManager::setAll(const SettingsData &data)
{
    instance().m_data = data;
    writeSettings();
}

void SettingsManager::writeSettings()
{
    QMutexLocker lock(&instance().m_mutex);
    if (instance().m_configFilePath.isEmpty()) {
#if SELF_CONTAINED_APP
        QString appDirPath = QCoreApplication::applicationDirPath();
        if (!appDirPath.isEmpty()) {
            appDirPath += "/";
        }
        QSettings settings(appDirPath + "config.ini", QSettings::IniFormat);
#else
        QSettings settings("Hobbits", "Hobbits GUI");
#endif
        instance().writeToSettings(settings);
    }
    else {
        QSettings settings(instance().m_configFilePath, QSettings::IniFormat);
        instance().writeToSettings(settings);
    }
}

void SettingsManager::readSettings()
{
    if (instance().m_hasRead) {
        return;
    }
    QMutexLocker lock(&instance().m_mutex);

    if (instance().m_configFilePath.isEmpty()) {
        QSettings settings("Hobbits", "Hobbits GUI");
        instance().readFromSettings(settings);
    }
    else {
        QSettings settings(instance().m_configFilePath, QSettings::IniFormat);
        instance().readFromSettings(settings);
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
#if DEVELOPMENT_BUILD
        if (key == SettingsManager::PLUGIN_PATH_KEY) {
            continue;
        }
#endif
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
#if DEVELOPMENT_BUILD
        if (key == SettingsManager::PLUGIN_PATH_KEY) {
            continue;
        }
#endif
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

QVariant SettingsManager::getTransientSetting(const QString &key)
{
    return instance().m_data.getTransientSetting(key);
}

void SettingsManager::setTransientSetting(const QString &key, const QVariant &value)
{
    instance().m_data.setTransientSetting(key, value);
}

QVariant SettingsManager::getPrivateSetting(const QString &key)
{
    readSettings();
    return instance().m_data.getPrivateSetting(key);

}

void SettingsManager::setPrivateSetting(const QString &key, const QVariant &value)
{
    instance().m_data.setPrivateSetting(key, value);
    writeSettings();
}

QVariant SettingsManager::getUiSetting(const QString &key)
{
    readSettings();
    return instance().m_data.getUiSetting(key);

}

void SettingsManager::setUiSetting(const QString &key, const QVariant &value)
{
    instance().m_data.setUiSetting(key, value);
    writeSettings();
}

QVariant SettingsManager::getPluginLoaderSetting(const QString &key)
{
    readSettings();
    return instance().m_data.getPluginLoaderSetting(key);

}

void SettingsManager::setPluginLoaderSetting(const QString &key, const QVariant &value)
{
    instance().m_data.setPluginLoaderSetting(key, value);
    writeSettings();
}

QVariant SettingsManager::getPluginSetting(const QString &key)
{
    readSettings();
    return instance().m_data.getPluginSetting(key);
}

void SettingsManager::setPluginSetting(const QString &key, const QVariant &value)
{
    instance().m_data.setPluginSetting(key, value);
    writeSettings();
}
