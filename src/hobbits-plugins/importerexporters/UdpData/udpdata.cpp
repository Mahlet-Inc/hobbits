#include "udpdata.h"
#include "udpreceiver.h"
#include "udpsender.h"
#include <QTimer>

UdpData::UdpData()
{
    QList<ParameterDelegate::ParameterInfo> importInfos = {
        {"port", ParameterDelegate::ParameterType::Integer},
        {"max_kb", ParameterDelegate::ParameterType::Integer},
        {"timeout", ParameterDelegate::ParameterType::Integer}
    };

    m_importDelegate = ParameterDelegate::create(
                    importInfos,
                    [](const Parameters &parameters) {
                        return QString("UDP Listen on port %1").arg(parameters.value("port").toInt());
                    },
                    [](QSharedPointer<ParameterDelegate> delegate, QSize size) {
                        Q_UNUSED(size)
                        return new UdpReceiver(delegate);
                    });


    QList<ParameterDelegate::ParameterInfo> exportInfos = {
        {"host", ParameterDelegate::ParameterType::String},
        {"port", ParameterDelegate::ParameterType::Integer}
    };
    m_exportDelegate = ParameterDelegate::create(
                    exportInfos,
                    [](const Parameters &parameters) {
                        return QString("UDP Send to %1:%2")
                                .arg(parameters.value("host").toString())
                                .arg(parameters.value("port").toInt());
                    },
                    [](QSharedPointer<ParameterDelegate> delegate, QSize size) {
                        Q_UNUSED(size)
                        return new UdpSender(delegate);
                    });
}

ImporterExporterInterface* UdpData::createDefaultImporterExporter()
{
    return new UdpData();
}

QString UdpData::name()
{
    return "UDP Data";
}

QString UdpData::description()
{
    return "UDP Socket Importing and Exporting";
}

QStringList UdpData::tags()
{
    return {"Generic", "Network"};
}

bool UdpData::canExport()
{
    return true;
}

bool UdpData::canImport()
{
    return true;
}

QSharedPointer<ParameterDelegate> UdpData::importParameterDelegate()
{
    return m_importDelegate;
}

QSharedPointer<ParameterDelegate> UdpData::exportParameterDelegate()
{
    return m_exportDelegate;
}

QSharedPointer<ImportResult> UdpData::importBits(const Parameters &parameters,
                                                 QSharedPointer<PluginActionProgress> progress)
{
    QStringList invalidations = m_importDelegate->validate(parameters);
    if (!invalidations.isEmpty()) {
        return ImportResult::error(QString("Invalid parameters passed to %1:\n%2").arg(name()).arg(invalidations.join("\n")));
    }

    return UdpReceiver::importData(parameters, progress);
}

QSharedPointer<ExportResult> UdpData::exportBits(QSharedPointer<const BitContainer> container,
                                                 const Parameters &parameters,
                                                 QSharedPointer<PluginActionProgress> progress)
{
    QStringList invalidations = m_exportDelegate->validate(parameters);
    if (!invalidations.isEmpty()) {
        return ExportResult::error(QString("Invalid parameters passed to %1:\n%2").arg(name()).arg(invalidations.join("\n")));
    }

    return UdpSender::exportData(container->bits(), parameters, progress);
}

