#include "packetcapture.h"
#include "packetcaptureclient.h"
#include "packetcaptureform.h"

PacketCapture::PacketCapture()
{
    QList<ParameterDelegate::ParameterInfo> importInfos = {
        {"device_name", QJsonValue::String},
        {"address", QJsonValue::Double},
        {"max_packets", QJsonValue::Double, true},
        {"max_size", QJsonValue::Double, true},
        {"timeout", QJsonValue::Double},
        {"filter", QJsonValue::String, true},
        {"buffer_size", QJsonValue::Double, true},
        {"packet_size_limit", QJsonValue::Double, true},
        {"promiscuous", QJsonValue::Bool}
    };

    m_importDelegate = QSharedPointer<ParameterDelegateUi>(
                new ParameterDelegateUi(
                    importInfos,
                    [](const QJsonObject &parameters) {
                        QString device = parameters.value("device_name").toString();
                        QString filter = parameters.value("filter").toString();

                        return QString("Packet Capture %1 %2").arg(device).arg(filter);
                    },
                    [](QSharedPointer<ParameterDelegate> delegate, QSize size) {
                        Q_UNUSED(size)
                        return new PacketCaptureForm(delegate);
                    }));
}

ImporterExporterInterface* PacketCapture::createDefaultImporterExporter()
{
    return new PacketCapture();
}

QString PacketCapture::name()
{
    return "Packet Capture";
}

QString PacketCapture::description()
{
    return "Captures packets from a network device";
}

QStringList PacketCapture::tags()
{
    return {"Generic", "Network"};
}

bool PacketCapture::canExport()
{
    return false;
}

bool PacketCapture::canImport()
{
    return true;
}

QSharedPointer<ParameterDelegate> PacketCapture::importParameterDelegate()
{
    return m_importDelegate;
}

QSharedPointer<ParameterDelegate> PacketCapture::exportParameterDelegate()
{
    return nullptr;
}

QSharedPointer<ImportResult> PacketCapture::importBits(QJsonObject parameters,
                                                       QSharedPointer<PluginActionProgress> progress)
{
    if (!m_importDelegate->validate(parameters)) {
        return ImportResult::error("Invalid parameters passed to Packet Capture");
    }

    return PacketCaptureClient::capturePackets(parameters, progress);
}

QSharedPointer<ExportResult> PacketCapture::exportBits(QSharedPointer<const BitContainer> container,
                                                       QJsonObject parameters,
                                                       QSharedPointer<PluginActionProgress> progress)
{
    Q_UNUSED(container)
    Q_UNUSED(parameters)
    Q_UNUSED(progress)
    return ExportResult::error("Export not implemented");
}
