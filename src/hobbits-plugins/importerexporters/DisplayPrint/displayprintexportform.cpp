#include "displayprintexportform.h"
#include "ui_displayprintexportform.h"
#include <QFileDialog>
#include "settingsmanager.h"

DisplayPrintExportForm::DisplayPrintExportForm(QSharedPointer<ParameterDelegate> delegate):
    ui(new Ui::DisplayPrintExportForm()),
    m_paramHelper(new ParameterHelper(delegate))
{
    ui->setupUi(this);

    m_paramHelper->addComboBoxParameter("plugin_name", ui->cb_pluginName);
    m_paramHelper->addSpinBoxIntParameter("image_width", ui->sb_width);
    m_paramHelper->addSpinBoxIntParameter("image_height", ui->sb_height);
    m_paramHelper->addLineEditStringParameter("image_filename", ui->le_file);

    auto manager = loadUpPluginManager();
    for (auto display : manager->displays()) {
        ui->cb_pluginName->addItem(display->name(), display->name());
    }
}

DisplayPrintExportForm::~DisplayPrintExportForm()
{
    delete ui;
}

QString DisplayPrintExportForm::title()
{
    return "Configure Display Print";
}

QJsonObject DisplayPrintExportForm::parameters()
{
    return m_paramHelper->getParametersFromUi();
}

QSharedPointer<HobbitsPluginManager> DisplayPrintExportForm::loadUpPluginManager()
{
    // TODO: this should be some kind of hobbits core functionality
    QSharedPointer<HobbitsPluginManager> manager(new HobbitsPluginManager());

    QStringList warnings;
    QStringList pluginPaths;
    pluginPaths.append(
            SettingsManager::getPluginLoaderSetting(
                    SettingsManager::PLUGIN_PATH_KEY).toString().split(":"));

    QStringList pathBuffer;
    for (QString pluginPath : pluginPaths) {

        if (pluginPath.startsWith("~/")) {
            pluginPath.replace(0, 1, QDir::homePath());
        }
        else if (!pluginPath.startsWith("/")) {
            pluginPath = QApplication::applicationDirPath() + "/" + pluginPath;
        }
        pathBuffer.append(pluginPath);
    }
    pluginPaths = pathBuffer;

    for (QString pluginPath : pluginPaths) {
        manager->loadPlugins(pluginPath);
    }

    return manager;
}

bool DisplayPrintExportForm::setParameters(QJsonObject parameters)
{
    return m_paramHelper->applyParametersToUi(parameters);
}

void DisplayPrintExportForm::on_tb_selectFile_clicked()
{
    QString file = QFileDialog::getSaveFileName(this, "Select Output Image File", QDir::homePath(), tr("PNG Image (*.png)"));

    if (file.isEmpty()) {
        return;
    }

    ui->le_file->setText(file);
}
