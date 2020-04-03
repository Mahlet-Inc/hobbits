#ifndef SETTINGSDATA_H
#define SETTINGSDATA_H

#include <QMap>
#include <QVariant>

#include "hobbits-core_global.h"

class HOBBITSCORESHARED_EXPORT SettingsData
{
public:
    SettingsData();
    SettingsData(const SettingsData &other);

    SettingsData& operator=(const SettingsData&);

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
    static const QString PLUGIN_PATH_KEY;
    static const QString PLUGIN_BLACKLIST_KEY;
    static const QString OPERATOR_DISPLAY_ORDER_KEY;
    static const QString ANALYZER_DISPLAY_ORDER_KEY;
    static const QString DISPLAY_DISPLAY_ORDER_KEY;
    static const QString LAST_TEMPLATE_PATH_KEY;
    static const QString LAST_IMPORT_EXPORT_PATH_KEY;
    static const QString LAST_CONTAINER_PATH_KEY;
    static const QString PLUGIN_RUNNING_KEY;

private:
    QMap<QString, QVariant> m_privateSettings;
    QMap<QString, QVariant> m_uiSettings;
    QMap<QString, QVariant> m_pluginLoaderSettings;
    QMap<QString, QVariant> m_pluginSettings;
};

#endif // SETTINGSDATA_H
