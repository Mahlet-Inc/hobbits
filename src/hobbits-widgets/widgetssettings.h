#ifndef WIDGETSSETTINGS_H
#define WIDGETSSETTINGS_H

#include "hobbits-widgets_global.h"
#include <QWidget>
#include <QFileDialog>

class HOBBITSWIDGETSSHARED_EXPORT WidgetsSettings
{
public:
    WidgetsSettings();

    static QString dialogDirKey(QString baseKey);
    static QString dialogSizeKey(QString baseKey);
    static QString dialogRectKey(QString baseKey);

    static QString getFile(
        QWidget *parent,
        const QString &caption,
        const QString &defaultDirectory,
        const QString &filter,
        QFileDialog::AcceptMode acceptMode,
        QFileDialog::FileMode fileMode,
        const QString &dialogSettingsKey);
};

#endif // WIDGETSSETTINGS_H
