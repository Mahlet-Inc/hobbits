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
    else if (pluginState.contains("hex_string")) {
        QString hexString = pluginState.value("hex_string").toString();
        if (hexString.size() > 16) {
            hexString.truncate(12);
            hexString += "...";
        }
        return QString("Import hex '%1'").arg(hexString);
    }
    return "";
}

QString HexString::getExportLabelForState(QJsonObject pluginState)
{
    Q_UNUSED(pluginState)
    return "";
}

QSharedPointer<ImportResult> HexString::importBits(QJsonObject pluginState)
{
    QSharedPointer<BitContainer> container;

    auto importer = QSharedPointer<HexStringImporter>(new HexStringImporter());

    if (pluginState.contains("filename")) {
        QString fileName = pluginState.value("filename").toString();
        return importer->importFromFile(fileName);
    }
    else if (pluginState.contains("hex_string")) {
        QString hexString = pluginState.value("hex_string").toString();
        int repeats = 1;
        if (pluginState.contains("repeats")) {
            repeats = pluginState.value("repeats").toInt();
        }
        return importer->importFromHexString(hexString, repeats);
    }

    if (importer->exec()) {
        return importer->getResult();
    }

    return ImportResult::nullResult();
}

QSharedPointer<ExportResult> HexString::exportBits(QSharedPointer<const BitContainer> container, QJsonObject pluginState)
{
    QString fileName;

    if (pluginState.contains("filename")) {
        fileName = pluginState.value("filename").toString();
    }
    else {
        fileName = QFileDialog::getSaveFileName(
                nullptr,
                tr("Export Bits"),
                SettingsManager::getInstance().getPrivateSetting(SettingsData::LAST_IMPORT_EXPORT_PATH_KEY).toString(),
                tr("All Files (*)"));
    }
    QFile file(fileName);

    pluginState.remove("filename");
    pluginState.insert("filename", fileName);

    if (!file.open(QIODevice::Truncate | QIODevice::WriteOnly)) {
        ExportResult::error(QString("Failed to open export file: '%1'").arg(fileName));
    }
    SettingsManager::getInstance().setPrivateSetting(
            SettingsData::LAST_IMPORT_EXPORT_PATH_KEY,
            QFileInfo(file).dir().path());

    qint64 byteLen = container->bits()->sizeInBytes();
    qint64 offset = 0;
    while (offset < byteLen) {
        QByteArray bytes = container->bits()->readBytes(offset, qMin(byteLen - offset, 100000ll)).toHex(' ');
        if (bytes.size() < 1) {
            return ExportResult::error("Failed to export container as hex string - failure reading from container");
        }
        qint64 written = file.write(bytes.data());
        if (written < bytes.size()) {
            return ExportResult::error("Failed to export container as hex string - failure writing hex string to file");
        }
        offset += bytes.size();
    }
    file.close();

    return ExportResult::result(pluginState);
}
