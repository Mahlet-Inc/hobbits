#include "widgetssettings.h"
#include "settingsmanager.h"
#include <QColor>

static WidgetsSettings _instance;

WidgetsSettings::WidgetsSettings()
{
    SettingsManager::instance().m_data.setUiSetting(SettingsManager::ONE_COLOR_KEY, QColor(Qt::black));
    SettingsManager::instance().m_data.setUiSetting(SettingsManager::ZERO_COLOR_KEY, QColor(253, 254, 229));
    SettingsManager::instance().m_data.setUiSetting(SettingsManager::BYTE_HUE_SAT_KEY, QColor::fromHsl(120, 200, 128));
    SettingsManager::instance().m_data.setUiSetting(SettingsManager::FOCUS_COLOR_KEY, QColor(50, 190, 0, 85));
    SettingsManager::instance().m_data.setUiSetting(SettingsManager::HIGHLIGHT_1_COLOR_KEY, QColor(100, 220, 100, 85));
    SettingsManager::instance().m_data.setUiSetting(SettingsManager::HIGHLIGHT_2_COLOR_KEY, QColor(100, 0, 255, 50));
    SettingsManager::instance().m_data.setUiSetting(SettingsManager::HIGHLIGHT_3_COLOR_KEY, QColor(0, 150, 230, 100));
    SettingsManager::instance().m_data.setUiSetting(SettingsManager::HIGHLIGHT_4_COLOR_KEY, QColor(200, 140, 0, 100));
    SettingsManager::instance().m_data.setUiSetting(SettingsManager::HIGHLIGHT_5_COLOR_KEY, QColor(250, 50, 0, 100));
}
