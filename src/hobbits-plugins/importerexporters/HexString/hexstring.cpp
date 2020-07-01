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
        importer->importFromFile(fileName);
        if (importer->getContainer().isNull()) {
            return ImportResult::error(QString("Failed to import hex string data from: '%1'").arg(fileName));
        }
        else {
            return ImportResult::result(importer->getContainer(), pluginState);
        }
    }
    else if (pluginState.contains("hex_string")) {
        QString hexString = pluginState.value("hex_string").toString();
        int repeats = 1;
        if (pluginState.contains("repeats")) {
            repeats = pluginState.value("repeats").toInt();
        }
        importer->importFromHexString(hexString, repeats);
        if (importer->getContainer().isNull()) {
            return ImportResult::error(QString("Failed to import hex string data from: '%1'").arg(hexString));
        }
        else {
            return ImportResult::result(importer->getContainer(), pluginState);
        }
    }

    if (importer->exec()) {
        if (!importer->getFileName().isEmpty()) {
            pluginState.insert("filename", importer->getFileName());
        }
        else if (!importer->getHexString().isEmpty()) {
            pluginState.insert("hex_string", importer->getHexString());
            pluginState.insert("repeats", importer->getRepeats());
        }
        return ImportResult::result(importer->getContainer(), pluginState);
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

    int len = container->bits()->getPreviewSize() / 8 + (container->bits()->getPreviewSize() % 8 ? 1 : 0);
    QByteArray bytes = container->bits()->getPreviewBytes().mid(0, len).toHex(' ');
    file.write(bytes.data());
    file.close();

    return ExportResult::result(pluginState);
}
