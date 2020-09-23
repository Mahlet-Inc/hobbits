#include "widgetssettings.h"
#include "settingsmanager.h"
#include <QColor>

static WidgetsSettings _instance;

WidgetsSettings::WidgetsSettings()
{
    SettingsManager::getInstance().setUiSetting(SettingsData::ONE_COLOR_KEY, QColor(Qt::black));
    SettingsManager::getInstance().setUiSetting(SettingsData::ZERO_COLOR_KEY, QColor(253, 254, 229));
    SettingsManager::getInstance().setUiSetting(SettingsData::BYTE_HUE_SAT_KEY, QColor::fromHsl(120, 200, 128));
    SettingsManager::getInstance().setUiSetting(SettingsData::FOCUS_COLOR_KEY, QColor(50, 190, 0, 85));
    SettingsManager::getInstance().setUiSetting(SettingsData::HIGHLIGHT_1_COLOR_KEY, QColor(100, 220, 100, 85));
    SettingsManager::getInstance().setUiSetting(SettingsData::HIGHLIGHT_2_COLOR_KEY, QColor(100, 0, 255, 50));
    SettingsManager::getInstance().setUiSetting(SettingsData::HIGHLIGHT_3_COLOR_KEY, QColor(0, 150, 230, 100));
    SettingsManager::getInstance().setUiSetting(SettingsData::HIGHLIGHT_4_COLOR_KEY, QColor(200, 140, 0, 100));
    SettingsManager::getInstance().setUiSetting(SettingsData::HIGHLIGHT_5_COLOR_KEY, QColor(250, 50, 0, 100));
}
