#ifndef PREFERENCESDIALOG_H
#define PREFERENCESDIALOG_H

#include "hobbitspluginmanager.h"
#include "settingsdata.h"
#include <QDialog>
#include <QLayoutItem>

namespace Ui
{
class PreferencesDialog;
}

class PreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PreferencesDialog(QSharedPointer<const HobbitsPluginManager> pluginManager, QWidget *parent = nullptr);

    ~PreferencesDialog();

private slots:
    void on_pb_apply_clicked();

    void on_pb_cancel_clicked();

    void on_pb_ok_clicked();

    void on_lw_groups_currentRowChanged(int currentRow);

    QWidget* createUiPage();
    QWidget* createPluginPage();
    QWidget* createPluginLoaderPage();

    QLayoutItem* createEditor(std::function<QVariant()> getter, std::function<void(QVariant)> setter, QWidget *parent);

private:
    static QPixmap getColorPixmap(QColor color);

    Ui::PreferencesDialog *ui;

    SettingsData m_data;
    QSharedPointer<const HobbitsPluginManager> m_pluginManager;
};

#endif // PREFERENCESDIALOG_H
