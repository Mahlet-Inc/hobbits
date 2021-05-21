#include "packetcapture.h"
#include "packetcaptureclient.h"
#include "packetcaptureform.h"

PacketCapture::PacketCapture()
{
    QList<ParameterDelegate::ParameterInfo> importInfos = {
        {"device_name", ParameterDelegate::ParameterType::String},
        {"address", ParameterDelegate::ParameterType::Integer},
        {"max_packets", ParameterDelegate::ParameterType::Integer, true},
        {"max_size", ParameterDelegate::ParameterType::Integer, true},
        {"timeout", ParameterDelegate::ParameterType::Integer},
        {"filter", ParameterDelegate::ParameterType::String, true},
        {"buffer_size", ParameterDelegate::ParameterType::Integer, true},
        {"packet_size_limit", ParameterDelegate::ParameterType::Integer, true},
        {"promiscuous", ParameterDelegate::ParameterType::Boolean}
    };

    m_importDelegate = ParameterDelegate::create(
                    importInfos,
                    [](const Parameters &parameters) {
                        QString device = parameters.value("device_name").toString();
                        QString filter = parameters.value("filter").toString();

                        return QString("Packet Capture %1 %2").arg(device).arg(filter);
                    },
                    [](QSharedPointer<ParameterDelegate> delegate, QSize size) {
                        Q_UNUSED(size)
                        return new PacketCaptureForm(delegate);
                    });
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

QSharedPointer<ImportResult> PacketCapture::importBits(const Parameters &parameters,
                                                       QSharedPointer<PluginActionProgress> progress)
{
    QStringList invalidations = m_importDelegate->validate(parameters);
    if (!invalidations.isEmpty()) {
        return ImportResult::error(QString("Invalid parameters passed to %1:\n%2").arg(name()).arg(invalidations.join("\n")));
    }

    return PacketCaptureClient::capturePackets(parameters, progress);
}

QSharedPointer<ExportResult> PacketCapture::exportBits(QSharedPointer<const BitContainer> container,
                                                       const Parameters &parameters,
                                                       QSharedPointer<PluginActionProgress> progress)
{
    Q_UNUSED(container)
    Q_UNUSED(parameters)
    Q_UNUSED(progress)
    return ExportResult::error("Export not implemented");
}
