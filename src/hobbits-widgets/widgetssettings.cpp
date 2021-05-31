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

QString WidgetsSettings::dialogDirKey(QString baseKey) 
{
    return baseKey + "_dir";
}

QString WidgetsSettings::dialogSizeKey(QString baseKey) 
{
    return baseKey + "_size";
}

QString WidgetsSettings::dialogRectKey(QString baseKey) 
{
    return baseKey + "_rect";
}

QString WidgetsSettings::getFile(QWidget *parent,
        const QString &caption,
        const QString &defaultDirectory,
        const QString &filter,
        QFileDialog::AcceptMode acceptMode,
        QFileDialog::FileMode fileMode,
        const QString &dialogSettingsKey) 
{
    QScopedPointer<QFileDialog> dialog(
        new QFileDialog(parent,
            caption,
            defaultDirectory,
            filter));
    
    dialog->setAcceptMode(acceptMode);
    dialog->setFileMode(fileMode);

    QString dirKey = dialogDirKey(dialogSettingsKey);
    QString rectKey = dialogRectKey(dialogSettingsKey);

    QVariant lastDir = SettingsManager::getPrivateSetting(dirKey);
    if (lastDir.isValid() && lastDir.canConvert<QString>()) {
        dialog->setDirectory(lastDir.toString());
    }

    QVariant lastRect = SettingsManager::getPrivateSetting(rectKey);
    if (lastRect.isValid() && lastRect.canConvert<QRect>()) {
        dialog->setGeometry(lastRect.toRect());
    }

    QString fileName;
    if (dialog->exec()) {
        QStringList fileNames = dialog->selectedFiles();
        if (!fileNames.isEmpty()) {
            fileName = fileNames.at(0);
        }
    }

    SettingsManager::setPrivateSetting(dirKey, dialog->directory().path());
    SettingsManager::setPrivateSetting(rectKey, dialog->geometry());

    return fileName;
}
