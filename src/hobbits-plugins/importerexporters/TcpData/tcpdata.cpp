#include "tcpdata.h"
#include "tcpreceiver.h"
#include "tcpsender.h"

TcpData::TcpData()
{

}

TcpData::~TcpData()
{
}

ImportExportInterface* TcpData::createDefaultImporterExporter()
{
    return new TcpData();
}

QString TcpData::getName()
{
    return "TCP Data";
}

bool TcpData::canExport()
{
    return true;
}

bool TcpData::canImport()
{
    return true;
}

QString TcpData::getImportLabelForState(QJsonObject pluginState)
{
    if (pluginState.contains("port") && pluginState.value("port").isDouble()) {
        return QString("TCP Listen on port %1").arg(pluginState.value("port").toInt());
    }
    return "";
}

QString TcpData::getExportLabelForState(QJsonObject pluginState)
{
    if (pluginState.contains("port") && pluginState.value("port").isDouble()
            && pluginState.contains("host") && pluginState.value("host").isString()) {
        return QString("TCP Export to %1 on port %2").arg(pluginState.value("host").toString()).arg(pluginState.value("port").toInt());
    }
    return "";
}

QSharedPointer<ImportResult> TcpData::importBits(QJsonObject pluginState)
{
    QSharedPointer<TcpReceiver> receiver = QSharedPointer<TcpReceiver>(new TcpReceiver());
    if (pluginState.contains("port") && pluginState.value("port").isDouble()) {
        receiver->setPort(pluginState.value("port").toInt());
        receiver->startListening();
    }

    if (receiver->exec()) {
        QSharedPointer<BitContainer> container = QSharedPointer<BitContainer>(new BitContainer());
        container->setBits(receiver->getDownloadedData());
        container->setName("TCP Import");
        QJsonObject state;
        state.insert("port", receiver->getPort());
        return ImportResult::result(container, state);
    }
    else {
        return ImportResult::error(receiver->getError());
    }
}

QSharedPointer<ExportResult> TcpData::exportBits(
        QSharedPointer<const BitContainer> container,
        QJsonObject pluginState)
{
    QSharedPointer<TcpSender> sender = QSharedPointer<TcpSender>(new TcpSender(container->bits()));
    if (pluginState.contains("port") && pluginState.value("port").isDouble()
            && pluginState.contains("host") && pluginState.value("host").isString()) {
        sender->setPort(pluginState.value("port").toInt());
        sender->setHost(pluginState.value("host").toString());
        sender->sendData();
    }

    if (sender->exec()) {
        QJsonObject state;
        state.insert("port", sender->getPort());
        state.insert("host", sender->getHost());
        return ExportResult::result(state);
    }
    else {
        return ExportResult::error("Failed to export bits over TCP");
    }
}
