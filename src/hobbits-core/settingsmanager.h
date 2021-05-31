#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QSettings>
#include <QMutex>
#include "settingsdata.h"
#include "hobbits-core_global.h"

class WidgetsSettings;

/**
  * @brief The SettingsManager class provides a singleton for reading and writings hobbits settings
  *
  * \see SettingsData
*/
class HOBBITSCORESHARED_EXPORT SettingsManager
{
    friend WidgetsSettings;
public:
    static void setConfigFilePath(const QString &configFilePath);

    static void setAll(const SettingsData &data);
    static SettingsData getAll();

    static void writeSettings();
    static void readSettings();

    static QVariant getTransientSetting(const QString &key);
    static void setTransientSetting(const QString &key, const QVariant &value);

    static QVariant getPrivateSetting(const QString &key);
    static void setPrivateSetting(const QString &key, const QVariant &value);

    static QVariant getUiSetting(const QString &key);
    static void setUiSetting(const QString &key, const QVariant &value);

    static QVariant getPluginLoaderSetting(const QString &key);
    static void setPluginLoaderSetting(const QString &key, const QVariant &value);

    static QVariant getPluginSetting(const QString &key);
    static void setPluginSetting(const QString &key, const QVariant &value);

private:
    static SettingsManager& instance();
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

    static const QString ONE_COLOR_KEY;
    static const QString ZERO_COLOR_KEY;
    static const QString BYTE_HUE_SAT_KEY;
    static const QString FOCUS_COLOR_KEY;
    static const QString HIGHLIGHT_1_COLOR_KEY;
    static const QString HIGHLIGHT_2_COLOR_KEY;
    static const QString HIGHLIGHT_3_COLOR_KEY;
    static const QString HIGHLIGHT_4_COLOR_KEY;
    static const QString HIGHLIGHT_5_COLOR_KEY;
    static const QString WINDOW_SIZE_KEY;
    static const QString WINDOW_POSITION_KEY;
    static const QString WINDOW_STATE_KEY;
    static const QString PLUGIN_PATH_KEY;
    static const QString PLUGIN_BLACKLIST_KEY;
    static const QString OPERATOR_DISPLAY_ORDER_KEY;
    static const QString ANALYZER_DISPLAY_ORDER_KEY;
    static const QString DISPLAY_DISPLAY_ORDER_KEY;
    static const QString LAST_BATCH_KEY;
    static const QString LAST_IMPORT_EXPORT_KEY;
    static const QString PLUGINS_RUNNING_KEY;
    static const QString PYTHON_HOME_KEY;
};

#endif // SETTINGSMANAGER_H
