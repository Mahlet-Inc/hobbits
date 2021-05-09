#include "settingsdata.h"
#include "settingsmanager.h"
#include <QDir>
#include <QPoint>
#include <QSize>
#include <QCoreApplication>
#include "hobbitscoreconfig.h"

SettingsData::SettingsData() :
    m_initialized(false)
{
}

SettingsData::SettingsData(const SettingsData &other):
    m_initialized(true)
{
    m_privateSettings = other.m_privateSettings;
    m_uiSettings = other.m_uiSettings;
    m_pluginSettings = other.m_pluginSettings;
    m_pluginLoaderSettings = other.m_pluginLoaderSettings;
}

SettingsData& SettingsData::operator=(const SettingsData &other)
{
    m_initialized = true;
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
    initialize();
    return m_transientSettings.value(key, defaultValue);
}

void SettingsData::setTransientSetting(const QString &key, const QVariant &value)
{
    initialize();
    m_transientSettings.remove(key);
    m_transientSettings.insert(key, value);
}

QList<QString> SettingsData::getPrivateSettingKeys()
{
    initialize();
    return m_privateSettings.keys();
}

QVariant SettingsData::getPrivateSetting(const QString &key, const QVariant &defaultValue)
{
    initialize();
    return m_privateSettings.value(key, defaultValue);
}

void SettingsData::setPrivateSetting(const QString &key, const QVariant &value)
{
    QMutexLocker lock(&m_mutex);
    initialize();
    m_privateSettings.remove(key);
    m_privateSettings.insert(key, value);
}

QList<QString> SettingsData::getUiSettingKeys()
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

QList<QString> SettingsData::getPluginLoaderSettingKeys()
{
    initialize();
    return m_pluginLoaderSettings.keys();
}

QVariant SettingsData::getPluginLoaderSetting(const QString &key, const QVariant &defaultValue)
{
    initialize();
    return m_pluginLoaderSettings.value(key, defaultValue);
}

void SettingsData::setPluginLoaderSetting(const QString &key, const QVariant &value)
{
    QMutexLocker lock(&m_mutex);
    initialize();
    m_pluginLoaderSettings.remove(key);
    m_pluginLoaderSettings.insert(key, value);
}

QList<QString> SettingsData::getPluginSettingKeys()
{
    initialize();
    return m_pluginSettings.keys();
}

QVariant SettingsData::getPluginSetting(const QString &key, const QVariant &defaultValue)
{
    initialize();
    return m_pluginSettings.value(key, defaultValue);
}

void SettingsData::setPluginSetting(const QString &key, const QVariant &value)
{
    QMutexLocker lock(&m_mutex);
    initialize();
    m_pluginSettings.remove(key);
    m_pluginSettings.insert(key, value);
}

void SettingsData::initialize()
{
    if (m_initialized) {
        return;
    }
    m_initialized = true;

    QString pythonHome;
    QString appDirPath = QCoreApplication::applicationDirPath();
    if (!appDirPath.isEmpty()) {
        appDirPath += "/";
    }
#ifdef Q_OS_LINUX
    #if SELF_CONTAINED_APP
    pythonHome = appDirPath + "../hobbits-cpython";
    #else
    pythonHome = "";
    #endif
#endif
#ifdef Q_OS_MACOS
    pythonHome = appDirPath + "../Frameworks/hobbits-cpython";
#endif
#ifdef Q_OS_WIN
    pythonHome = appDirPath;
#endif
    QString pythonHomeCanonical = QDir(pythonHome).canonicalPath();
    // The canonical path will be empty if it doesn't exist, so we'll fall back on the base path for easier debugging
    if (!pythonHomeCanonical.isEmpty()) {
        pythonHome = pythonHomeCanonical;
    }
    m_transientSettings.insert(SettingsManager::PYTHON_HOME_KEY, pythonHome);

    m_privateSettings.insert(SettingsManager::WINDOW_SIZE_KEY, QSize(640, 480));
    m_privateSettings.insert(SettingsManager::WINDOW_POSITION_KEY, QPoint(100, 100));
    m_privateSettings.insert(SettingsManager::LAST_BATCH_PATH_KEY, QDir::homePath());
    m_privateSettings.insert(SettingsManager::LAST_IMPORT_EXPORT_PATH_KEY, QDir::homePath());
    m_privateSettings.insert(SettingsManager::LAST_CONTAINER_PATH_KEY, QDir::homePath());

    m_pluginLoaderSettings.insert(SettingsManager::PLUGIN_PATH_KEY, HobbitsCoreConfig::PLUGINS_DEFAULT_PATH);
    m_pluginLoaderSettings.insert(SettingsManager::PLUGIN_BLACKLIST_KEY, QStringList({}));
    m_pluginLoaderSettings.insert(
            SettingsManager::OPERATOR_DISPLAY_ORDER_KEY,
            QStringList({"Take Skip", "Header Framer", "Bit Error", "LFSR"}));
    m_pluginLoaderSettings.insert(
            SettingsManager::ANALYZER_DISPLAY_ORDER_KEY,
            QStringList({ "Find", "Width Framer", "Flexible Framer" }));
    m_pluginLoaderSettings.insert(SettingsManager::DISPLAY_DISPLAY_ORDER_KEY, QStringList({"Bit Raster", "Hex", "Binary", "ASCII"}));
}
