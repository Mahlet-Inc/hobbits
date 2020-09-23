#include "settingsdata.h"

#include <QDir>
#include <QPoint>
#include <QSize>
#include <QCoreApplication>

const QString SettingsData::ONE_COLOR_KEY = "1-Bit Color";
const QString SettingsData::ZERO_COLOR_KEY = "0-Bit Color";
const QString SettingsData::BYTE_HUE_SAT_KEY = "Byte Hue and Saturation";
const QString SettingsData::FOCUS_COLOR_KEY = "Range Focus Color";
const QString SettingsData::HIGHLIGHT_1_COLOR_KEY = "Highlight 1";
const QString SettingsData::HIGHLIGHT_2_COLOR_KEY = "Highlight 2";
const QString SettingsData::HIGHLIGHT_3_COLOR_KEY = "Highlight 3";
const QString SettingsData::HIGHLIGHT_4_COLOR_KEY = "Highlight 4";
const QString SettingsData::HIGHLIGHT_5_COLOR_KEY = "Highlight 5";
const QString SettingsData::PLUGIN_PATH_KEY = "Plugin Path";
const QString SettingsData::PLUGIN_BLACKLIST_KEY = "Plugin Blacklist";
const QString SettingsData::OPERATOR_DISPLAY_ORDER_KEY = "Operator Display Order";
const QString SettingsData::ANALYZER_DISPLAY_ORDER_KEY = "Analyzer Display Order";
const QString SettingsData::DISPLAY_DISPLAY_ORDER_KEY = "Display Display Order";
const QString SettingsData::WINDOW_SIZE_KEY = "window_size";
const QString SettingsData::WINDOW_POSITION_KEY = "window_position";
const QString SettingsData::WINDOW_STATE_KEY = "window_state";
const QString SettingsData::LAST_BATCH_PATH_KEY = "last_batch_path";
const QString SettingsData::LAST_IMPORT_EXPORT_PATH_KEY = "last_import_export_path";
const QString SettingsData::LAST_CONTAINER_PATH_KEY = "last_container_path";
const QString SettingsData::PLUGIN_RUNNING_KEY = "plugin_running";
const QString SettingsData::PLUGINS_RUNNING_KEY = "plugins_running";
const QString SettingsData::PYTHON_HOME_KEY = "python_home_dir";

SettingsData::SettingsData()
{
    QString pythonHome;
#ifdef Q_OS_LINUX
    pythonHome = QCoreApplication::applicationDirPath()+"/../python";
#endif
#ifdef Q_OS_MACOS
    pythonHome = QCoreApplication::applicationDirPath()+"/../Frameworks/python";
#endif
#ifdef Q_OS_WIN
    pythonHome = QCoreApplication::applicationDirPath();
#endif
    QString pythonHomeCanonical = QDir(pythonHome).canonicalPath();
    // The canonical path will be empty if it doesn't exist, so we'll fall back on the base path for easier debugging
    if (!pythonHomeCanonical.isEmpty()) {
        pythonHome = pythonHomeCanonical;
    }
    m_transientSettings.insert(PYTHON_HOME_KEY, pythonHome);

    m_privateSettings.insert(WINDOW_SIZE_KEY, QSize(640, 480));
    m_privateSettings.insert(WINDOW_POSITION_KEY, QPoint(100, 100));
    m_privateSettings.insert(LAST_BATCH_PATH_KEY, QDir::homePath());
    m_privateSettings.insert(LAST_IMPORT_EXPORT_PATH_KEY, QDir::homePath());
    m_privateSettings.insert(LAST_CONTAINER_PATH_KEY, QDir::homePath());

    m_pluginLoaderSettings.insert(
            PLUGIN_PATH_KEY,
            "../hobbits-plugins:../plugins:plugins:~/.local/share/hobbits/plugins");
    m_pluginLoaderSettings.insert(PLUGIN_BLACKLIST_KEY, QStringList({}));
    m_pluginLoaderSettings.insert(
            OPERATOR_DISPLAY_ORDER_KEY,
            QStringList({"Take Skip", "Header Framer", "Bit Error", "LFSR"}));
    m_pluginLoaderSettings.insert(
            ANALYZER_DISPLAY_ORDER_KEY,
            QStringList({ "Find", "Width Framer", "Flexible Framer" }));
    m_pluginLoaderSettings.insert(DISPLAY_DISPLAY_ORDER_KEY, QStringList({"Bit Raster", "Hex", "Binary", "ASCII"}));

}

SettingsData::SettingsData(const SettingsData &other)
{
    m_privateSettings = other.m_privateSettings;
    m_uiSettings = other.m_uiSettings;
    m_pluginSettings = other.m_pluginSettings;
    m_pluginLoaderSettings = other.m_pluginLoaderSettings;
}

SettingsData& SettingsData::operator=(const SettingsData &other)
{
    if (this != &other) {
        m_privateSettings = other.m_privateSettings;
        m_uiSettings = other.m_uiSettings;
        m_pluginSettings = other.m_pluginSettings;
        m_pluginLoaderSettings = other.m_pluginLoaderSettings;
    }
    return *this;
}

QVariant SettingsData::getTransientSetting(const QString &key, const QVariant &defaultValue)
{
    return m_transientSettings.value(key, defaultValue);
}

void SettingsData::setTransientSetting(const QString &key, const QVariant &value)
{
    m_transientSettings.remove(key);
    m_transientSettings.insert(key, value);
}

QList<QString> SettingsData::getPrivateSettingKeys() const
{
    return m_privateSettings.keys();
}

QVariant SettingsData::getPrivateSetting(const QString &key, const QVariant &defaultValue)
{
    return m_privateSettings.value(key, defaultValue);
}

void SettingsData::setPrivateSetting(const QString &key, const QVariant &value)
{
    QMutexLocker lock(&m_mutex);
    m_privateSettings.remove(key);
    m_privateSettings.insert(key, value);
}

QList<QString> SettingsData::getUiSettingKeys() const
{
    return m_uiSettings.keys();
}

QVariant SettingsData::getUiSetting(const QString &key, const QVariant &defaultValue)
{
    return m_uiSettings.value(key, defaultValue);
}

void SettingsData::setUiSetting(const QString &key, const QVariant &value)
{
    QMutexLocker lock(&m_mutex);
    m_uiSettings.remove(key);
    m_uiSettings.insert(key, value);
}

QList<QString> SettingsData::getPluginLoaderSettingKeys() const
{
    return m_pluginLoaderSettings.keys();
}

QVariant SettingsData::getPluginLoaderSetting(const QString &key, const QVariant &defaultValue)
{
    return m_pluginLoaderSettings.value(key, defaultValue);
}

void SettingsData::setPluginLoaderSetting(const QString &key, const QVariant &value)
{
    QMutexLocker lock(&m_mutex);
    m_pluginLoaderSettings.remove(key);
    m_pluginLoaderSettings.insert(key, value);
}

QList<QString> SettingsData::getPluginSettingKeys() const
{
    return m_pluginSettings.keys();
}

QVariant SettingsData::getPluginSetting(const QString &key, const QVariant &defaultValue)
{
    return m_pluginSettings.value(key, defaultValue);
}

void SettingsData::setPluginSetting(const QString &key, const QVariant &value)
{
    QMutexLocker lock(&m_mutex);
    m_pluginSettings.remove(key);
    m_pluginSettings.insert(key, value);
}
