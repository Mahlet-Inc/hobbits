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

QSharedPointer<BitContainer> FileData::importBits(QMap<QString, QString> args, QWidget *parent)
{

    QSharedPointer<BitContainer> nullResult;

    QString fileName;

    if (args.contains("filename")) {
        fileName = args.value("filename");
    }
    else {
        fileName = QFileDialog::getOpenFileName(
                parent,
                tr("Import Bits"),
                SettingsManager::getInstance().getPrivateSetting(SettingsData::LAST_IMPORT_EXPORT_PATH_KEY).toString(),
                tr("All Files (*)"));
    }

    if (fileName.isEmpty()) {
        return nullResult;
    }

    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox msg(parent);
        msg.setWindowTitle("Import Bits Error");
        msg.setText(QString("Failed to import bit file: '%1'").arg(fileName));
        msg.setDefaultButton(QMessageBox::Ok);
        msg.exec();
        return nullResult;
    }
    SettingsManager::getInstance().setPrivateSetting(
            SettingsData::LAST_IMPORT_EXPORT_PATH_KEY,
            QFileInfo(file).dir().path());

    ImportBitsWizard *importWizard = new ImportBitsWizard(fileName, parent);

    importWizard->setModal(true);

    if (importWizard->exec() == QDialog::Accepted) {
        auto container = importWizard->getImportedBitContainer();
        delete importWizard;
        return container;
    }
    else {
        delete importWizard;
        return nullResult;
    }
}

void FileData::exportBits(QSharedPointer<const BitContainer> container, QMap<QString, QString> args, QWidget *parent)
{
    QString fileName;

    if (args.contains("filename")) {
        fileName = args.value("filename");
    }
    else {
        fileName = QFileDialog::getSaveFileName(
                parent,
                tr("Export Bits"),
                SettingsManager::getInstance().getPrivateSetting(SettingsData::LAST_IMPORT_EXPORT_PATH_KEY).toString(),
                tr("All Files (*)"));
    }
    QFile file(fileName);
    if (!file.open(QIODevice::Truncate | QIODevice::WriteOnly)) {
        return;
    }
    SettingsManager::getInstance().setPrivateSetting(
            SettingsData::LAST_IMPORT_EXPORT_PATH_KEY,
            QFileInfo(file).dir().path());

    QByteArray bytes = container->getBaseBits()->getBytes();
    qint64 len = container->getBaseBits()->size() / 8 + (container->getBaseBits()->size() % 8 ? 1 : 0);
    file.write(bytes.data(), len);
    file.close();
}
