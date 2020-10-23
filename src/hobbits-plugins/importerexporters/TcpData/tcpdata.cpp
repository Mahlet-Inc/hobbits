#include "tcpdata.h"
#include "tcpreceiver.h"
#include "tcpsender.h"

TcpData::TcpData()
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
                        return QString("TCP Listen on port %1").arg(parameters.value("port").toInt());
                    },
                    [](QSharedPointer<ParameterDelegate> delegate, QSize size) {
                        Q_UNUSED(size)
                        return new TcpReceiver(delegate);
                    }));


    QList<ParameterDelegate::ParameterInfo> exportInfos = {
        {"host", QJsonValue::String},
        {"port", QJsonValue::Double},
        {"timeout", QJsonValue::Double}
    };
    m_exportDelegate = QSharedPointer<ParameterDelegateUi>(
                new ParameterDelegateUi(
                    exportInfos,
                    [](const QJsonObject &parameters) {
                        return QString("TCP Send to %1:%2")
                                .arg(parameters.value("host").toString())
                                .arg(parameters.value("port").toInt());
                    },
                    [](QSharedPointer<ParameterDelegate> delegate, QSize size) {
                        Q_UNUSED(size)
                        return new TcpSender(delegate);
                    }));
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

QSharedPointer<ImportResult> TcpData::importBits(QJsonObject parameters,
                                                 QSharedPointer<PluginActionProgress> progress)
{
    if (!m_importDelegate->validate(parameters)) {
        return ImportResult::error("Invalid parameters passed to TCP Import");
    }

    return TcpReceiver::importData(parameters, progress);
}

QSharedPointer<ExportResult> TcpData::exportBits(QSharedPointer<const BitContainer> container,
                                                 QJsonObject parameters,
                                                 QSharedPointer<PluginActionProgress> progress)
{
    if (!m_exportDelegate->validate(parameters)) {
        return ExportResult::error("Invalid parameters passed to TCP Export");
    }

    return TcpSender::exportData(container->bits(), parameters, progress);
}
