#include "tcpdata.h"
#include "tcpreceiver.h"
#include "tcpsender.h"

TcpData::TcpData()
{
    QList<ParameterDelegate::ParameterInfo> importInfos = {
        {"port", ParameterDelegate::ParameterType::Integer},
        {"max_kb", ParameterDelegate::ParameterType::Integer},
        {"timeout", ParameterDelegate::ParameterType::Integer}
    };

    m_importDelegate = ParameterDelegate::create(
                    importInfos,
                    [](const Parameters &parameters) {
                        return QString("TCP Listen on port %1").arg(parameters.value("port").toInt());
                    },
                    [](QSharedPointer<ParameterDelegate> delegate, QSize size) {
                        Q_UNUSED(size)
                        return new TcpReceiver(delegate);
                    });


    QList<ParameterDelegate::ParameterInfo> exportInfos = {
        {"host", ParameterDelegate::ParameterType::String},
        {"port", ParameterDelegate::ParameterType::Integer},
        {"timeout", ParameterDelegate::ParameterType::Integer}
    };
    m_exportDelegate = ParameterDelegate::create(
                    exportInfos,
                    [](const Parameters &parameters) {
                        return QString("TCP Send to %1:%2")
                                .arg(parameters.value("host").toString())
                                .arg(parameters.value("port").toInt());
                    },
                    [](QSharedPointer<ParameterDelegate> delegate, QSize size) {
                        Q_UNUSED(size)
                        return new TcpSender(delegate);
                    });
}

ImporterExporterInterface* TcpData::createDefaultImporterExporter()
{
    return new TcpData();
}

QString TcpData::name()
{
    return "TCP Data";
}

QString TcpData::description()
{
    return "TCP Socket Importing and Exporting";
}

QStringList TcpData::tags()
{
    return {"Generic", "Network"};
}

bool TcpData::canExport()
{
    return true;
}

bool TcpData::canImport()
{
    return true;
}

QSharedPointer<ParameterDelegate> TcpData::importParameterDelegate()
{
    return m_importDelegate;
}

QSharedPointer<ParameterDelegate> TcpData::exportParameterDelegate()
{
    return m_exportDelegate;
}

QSharedPointer<ImportResult> TcpData::importBits(const Parameters &parameters,
                                                 QSharedPointer<PluginActionProgress> progress)
{
    QStringList invalidations = m_importDelegate->validate(parameters);
    if (!invalidations.isEmpty()) {
        return ImportResult::error(QString("Invalid parameters passed to %1:\n%2").arg(name()).arg(invalidations.join("\n")));
    }

    return TcpReceiver::importData(parameters, progress);
}

QSharedPointer<ExportResult> TcpData::exportBits(QSharedPointer<const BitContainer> container,
                                                 const Parameters &parameters,
                                                 QSharedPointer<PluginActionProgress> progress)
{
    QStringList invalidations = m_exportDelegate->validate(parameters);
    if (!invalidations.isEmpty()) {
        return ExportResult::error(QString("Invalid parameters passed to %1:\n%2").arg(name()).arg(invalidations.join("\n")));
    }

    return TcpSender::exportData(container->bits(), parameters, progress);
}
