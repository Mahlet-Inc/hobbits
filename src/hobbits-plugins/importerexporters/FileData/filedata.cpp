#include "filedata.h"
#include "importbitswizard.h"
#include "settingsmanager.h"
#include "ui_filedata.h"
#include <QFileDialog>
#include <QMessageBox>

FileData::FileData() :
    ui(new Ui::FileData())
{

}

FileData::~FileData()
{
    delete ui;
}

ImportExportInterface* FileData::createDefaultImporterExporter()
{
    return new FileData();
}

QString FileData::getName()
{
    return "File Data";
}

bool FileData::canExport()
{
    return true;
}

bool FileData::canImport()
{
    return true;
}

QString FileData::getImportLabelForState(QJsonObject pluginState)
{
    if (pluginState.contains("filename")) {
        QString fileName = pluginState.value("filename").toString();
        return QString("Import file %1").arg(fileName);
    }
    return "";
}

QString FileData::getExportLabelForState(QJsonObject pluginState)
{
    return "";
}

QSharedPointer<ImportExportResult> FileData::importBits(QJsonObject pluginState, QWidget *parent)
{
    QString fileName;

    if (pluginState.contains("filename")) {
        fileName = pluginState.value("filename").toString();
    }
    else {
        fileName = QFileDialog::getOpenFileName(
                parent,
                tr("Import Bits"),
                SettingsManager::getInstance().getPrivateSetting(SettingsData::LAST_IMPORT_EXPORT_PATH_KEY).toString(),
                tr("All Files (*)"));
    }

    if (fileName.isEmpty()) {
        return ImportExportResult::error("No file selected for import");
    }

    pluginState.remove("filename");
    pluginState.insert("filename", fileName);

    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox msg(parent);
        msg.setWindowTitle("Import Bits Error");
        msg.setText(QString("Failed to import bit file: '%1'").arg(fileName));
        msg.setDefaultButton(QMessageBox::Ok);
        msg.exec();
        return ImportExportResult::error(QString("Failed to import bit file: '%1'").arg(fileName));
    }
    SettingsManager::getInstance().setPrivateSetting(
            SettingsData::LAST_IMPORT_EXPORT_PATH_KEY,
            QFileInfo(file).dir().path());

    QSharedPointer<BitContainer> container = QSharedPointer<BitContainer>(new BitContainer);
    container->setBits(&file);
    container->setName(QFileInfo(file).fileName());

    return ImportExportResult::create(container, pluginState);
}

QSharedPointer<ImportExportResult> FileData::exportBits(QSharedPointer<const BitContainer> container, QJsonObject pluginState, QWidget *parent)
{
    QString fileName;

    if (pluginState.contains("filename")) {
        fileName = pluginState.value("filename").toString();
    }
    else {
        fileName = QFileDialog::getSaveFileName(
                parent,
                tr("Export Bits"),
                SettingsManager::getInstance().getPrivateSetting(SettingsData::LAST_IMPORT_EXPORT_PATH_KEY).toString(),
                tr("All Files (*)"));
    }
    pluginState.remove("filename");
    pluginState.insert("filename", fileName);

    QFile file(fileName);
    if (!file.open(QIODevice::Truncate | QIODevice::WriteOnly)) {
        return ImportExportResult::error(QString("Failed to open export bit file: '%1'").arg(fileName));
    }
    SettingsManager::getInstance().setPrivateSetting(
            SettingsData::LAST_IMPORT_EXPORT_PATH_KEY,
            QFileInfo(file).dir().path());

    container->bits()->writeTo(&file);
    file.close();

    return ImportExportResult::create(pluginState);
}
