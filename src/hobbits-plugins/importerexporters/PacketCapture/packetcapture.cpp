#include "packetcapture.h"
#include "capturedialog.h"

PacketCapture::PacketCapture()
{

}

PacketCapture::~PacketCapture()
{
}

ImportExportInterface* PacketCapture::createDefaultImporterExporter()
{
    return new PacketCapture();
}

QString PacketCapture::getName()
{
    return "Packet Capture";
}

bool PacketCapture::canExport()
{
    return false;
}

bool PacketCapture::canImport()
{
    return true;
}

QString PacketCapture::getImportLabelForState(QJsonObject pluginState)
{
    return "Packet Capture";
}

QString PacketCapture::getExportLabelForState(QJsonObject pluginState)
{
    return "";
}

QSharedPointer<ImportResult> PacketCapture::importBits(QJsonObject pluginState)
{
    QScopedPointer<CaptureDialog> capture(new CaptureDialog());
    if (!capture->exec()) {
        return ImportResult::nullResult();
    }

    return ImportResult::result(capture->container(), QJsonObject());
}

QSharedPointer<ExportResult> PacketCapture::exportBits(
        QSharedPointer<const BitContainer> container,
        QJsonObject pluginState)
{
    Q_UNUSED(container)
    Q_UNUSED(pluginState)
    return ExportResult::error("Export not implemented");
}
