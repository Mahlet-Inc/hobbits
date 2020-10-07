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

ImporterExporterInterface* UdpData::createDefaultImporterExporter()
{
    return new UdpData();
}

QString UdpData::name()
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

        if (pluginState.contains("limit") && pluginState.value("limit").isDouble()) {
            int value = pluginState.value("limit").toInt();
            if (value > 0) {
                return QString("UDP Listen for %2 KB on port %1").arg(pluginState.value("port").toInt()).arg(double(value)/1000.0);
            }
        }
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
        if (pluginState.contains("limit") && pluginState.value("limit").isDouble()) {
            receiver->setLimit(pluginState.value("limit").toInt());
        }
        receiver->startListening();
    }

    if (receiver->exec()) {
        QSharedPointer<BitContainer> container;
        if (receiver->getLimit() > 0) {
            container = BitContainer::create(receiver->getDownloadedData(), receiver->getLimit() * 8);
        }
        else {
            container = BitContainer::create(receiver->getDownloadedData());
        }
        container->setName("UDP Import");
        QJsonObject state;
        state.insert("port", receiver->getPort());
        state.insert("limit", receiver->getLimit());
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
