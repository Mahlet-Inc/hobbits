#include "udpdata.h"
#include "udpreceiver.h"
#include "udpsender.h"
#include <QTimer>

UdpData::UdpData()
{
    QList<ParameterDelegate::ParameterInfo> importInfos = {
        {"port", QJsonValue::Double},
        {"max_kb", QJsonValue::Double},
        {"timeout", QJsonValue::Double}
    };

    m_importDelegate = QSharedPointer<ParameterDelegateUi>(
                new ParameterDelegateUi(
                    importInfos,
                    [](const QJsonObject &parameters) {
                        return QString("UDP Listen on port %1").arg(parameters.value("port").toInt());
                    },
                    [](QSharedPointer<ParameterDelegate> delegate, QSize size) {
                        Q_UNUSED(size)
                        return new UdpReceiver(delegate);
                    }));


    QList<ParameterDelegate::ParameterInfo> exportInfos = {
        {"host", QJsonValue::String},
        {"port", QJsonValue::Double}
    };
    m_exportDelegate = QSharedPointer<ParameterDelegateUi>(
                new ParameterDelegateUi(
                    exportInfos,
                    [](const QJsonObject &parameters) {
                        return QString("UDP Send to %1:%2")
                                .arg(parameters.value("host").toString())
                                .arg(parameters.value("port").toInt());
                    },
                    [](QSharedPointer<ParameterDelegate> delegate, QSize size) {
                        Q_UNUSED(size)
                        return new UdpSender(delegate);
                    }));
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

QSharedPointer<ImportResult> UdpData::importBits(QJsonObject parameters,
                                                 QSharedPointer<PluginActionProgress> progress)
{
    if (!m_importDelegate->validate(parameters)) {
        return ImportResult::error("Invalid parameters passed to UDP Import");
    }

    return UdpReceiver::importData(parameters, progress);
}

QSharedPointer<ExportResult> UdpData::exportBits(QSharedPointer<const BitContainer> container,
                                                 QJsonObject parameters,
                                                 QSharedPointer<PluginActionProgress> progress)
{
    if (!m_exportDelegate->validate(parameters)) {
        return ExportResult::error("Invalid parameters passed to UDP Export");
    }

    return UdpSender::exportData(container->bits(), parameters, progress);
}

