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

QSharedPointer<ImportExportResult> TcpData::importBits(QJsonObject pluginState, QWidget *parent)
{
    QSharedPointer<TcpReceiver> receiver = QSharedPointer<TcpReceiver>(new TcpReceiver(parent));
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
        return ImportExportResult::create(container, state);
    }
    else {
        return ImportExportResult::error(receiver->getError());
    }
}

QSharedPointer<ImportExportResult> TcpData::exportBits(
        QSharedPointer<const BitContainer> container,
        QJsonObject pluginState,
        QWidget *parent)
{
    QSharedPointer<TcpSender> sender = QSharedPointer<TcpSender>(new TcpSender(container->bits(), parent));
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
        return ImportExportResult::create(state);
    }
    else {
        return ImportExportResult::error("Failed to export bits over TCP");
    }
}
