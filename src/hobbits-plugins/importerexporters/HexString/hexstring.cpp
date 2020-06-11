#include "hexstring.h"
#include "hexstringimporter.h"
#include "settingsmanager.h"
#include <QFileDialog>

HexString::HexString()
{

}

HexString::~HexString()
{
}

ImportExportInterface* HexString::createDefaultImporterExporter()
{
    return new HexString();
}

QString HexString::getName()
{
    return "Hex String";
}

bool HexString::canExport()
{
    return true;
}

bool HexString::canImport()
{
    return true;
}

QString HexString::getImportLabelForState(QJsonObject pluginState)
{
    if (pluginState.contains("filename")) {
        QString fileName = pluginState.value("filename").toString();
        return QString("Import hex from %1").arg(fileName);
    }
    return "";
}

QString HexString::getExportLabelForState(QJsonObject pluginState)
{
    Q_UNUSED(pluginState)
    return "";
}

QSharedPointer<ImportExportResult> HexString::importBits(QJsonObject pluginState, QWidget *parent)
{
    QSharedPointer<BitContainer> container;

    auto importer = QSharedPointer<HexStringImporter>(new HexStringImporter(parent));

    if (pluginState.contains("filename")) {
        QString fileName = pluginState.value("filename").toString();
        importer->importFromFile(fileName);
        if (importer->getContainer().isNull()) {
            return ImportExportResult::error(QString("Failed to import hex string data from: '%1'").arg(fileName));
        }
        else {
            return ImportExportResult::create(importer->getContainer(), pluginState);
        }
    }

    if (importer->exec()) {
        if (!importer->getFileName().isEmpty()) {
            pluginState.insert("filename", importer->getFileName());
        }
        return ImportExportResult::create(importer->getContainer(), pluginState);
    }

    return ImportExportResult::nullResult();
}

QSharedPointer<ImportExportResult> HexString::exportBits(QSharedPointer<const BitContainer> container, QJsonObject pluginState, QWidget *parent)
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
    QFile file(fileName);

    pluginState.remove("filename");
    pluginState.insert("filename", fileName);

    if (!file.open(QIODevice::Truncate | QIODevice::WriteOnly)) {
        ImportExportResult::error(QString("Failed to open export file: '%1'").arg(fileName));
    }
    SettingsManager::getInstance().setPrivateSetting(
            SettingsData::LAST_IMPORT_EXPORT_PATH_KEY,
            QFileInfo(file).dir().path());

    int len = container->bits()->getPreviewSize() / 8 + (container->bits()->getPreviewSize() % 8 ? 1 : 0);
    QByteArray bytes = container->bits()->getPreviewBytes().mid(0, len).toHex(' ');
    file.write(bytes.data());
    file.close();

    return ImportExportResult::create(pluginState);
}
