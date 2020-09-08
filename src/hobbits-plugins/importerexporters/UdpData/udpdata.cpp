#include "udpdata.h"
#include "udpreceiver.h"
#include "udpsender.h"
#include <QTimer>

UdpData::UdpData()
{

}

UdpData::~UdpData()
{
}

ImportExportInterface* UdpData::createDefaultImporterExporter()
{
    return new UdpData();
}

QString UdpData::getName()
{
    return "UDP Data";
}

bool UdpData::canExport()
{
    return true;
}

bool UdpData::canImport()
{
    return true;
}

QString UdpData::getImportLabelForState(QJsonObject pluginState)
{
    if (pluginState.contains("port") && pluginState.value("port").isDouble()) {
        return QString("UDP Listen on port %1").arg(pluginState.value("port").toInt());
    }
    return "";
}

QString UdpData::getExportLabelForState(QJsonObject pluginState)
{
    if (pluginState.contains("port") && pluginState.value("port").isDouble()
            && pluginState.contains("host") && pluginState.value("host").isString()) {
        return QString("UDP Export to %1 on port %2").arg(pluginState.value("host").toString()).arg(pluginState.value("port").toInt());
    }
    return "";
}

QSharedPointer<ImportResult> UdpData::importBits(QJsonObject pluginState)
{
    QSharedPointer<UdpReceiver> receiver = QSharedPointer<UdpReceiver>(new UdpReceiver());
    if (pluginState.contains("port") && pluginState.value("port").isDouble()) {
        receiver->setPort(pluginState.value("port").toInt());
        receiver->startListening();
    }

    if (receiver->exec()) {
        QSharedPointer<BitContainer> container = QSharedPointer<BitContainer>(new BitContainer());
        container->setBits(receiver->getDownloadedData());
        container->setName("UDP Import");
        QJsonObject state;
        state.insert("port", receiver->getPort());
        return ImportResult::result(container, state);
    }
    else {
        return ImportResult::error(receiver->getError());
    }
}

QSharedPointer<ExportResult> UdpData::exportBits(
        QSharedPointer<const BitContainer> container,
        QJsonObject pluginState)
{
    QSharedPointer<UdpSender> sender = QSharedPointer<UdpSender>(new UdpSender(container->bits()));
    if (pluginState.contains("port") && pluginState.value("port").isDouble()
            && pluginState.contains("host") && pluginState.value("host").isString()) {
        sender->setPort(pluginState.value("port").toInt());
        sender->setHost(pluginState.value("host").toString());
        QTimer::singleShot(10, sender.data(), SLOT(sendData()));
        if (sender->exec()) {
            return ExportResult::result(pluginState);
        }
        else {
            return ExportResult::error("Failed to export bits over UDP");
        }
    }

    if (sender->exec()) {
        QJsonObject state;
        state.insert("port", sender->getPort());
        state.insert("host", sender->getHost());
        return ExportResult::result(state);
    }
    else {
        return ExportResult::error("Failed to export bits over UDP");
    }
}
