#include "packetcaptureclient.h"
#include <QMessageBox>
#include <QStandardItem>
#include <QHostAddress>
#include <QMetaObject>
#include <QtConcurrent/QtConcurrentRun>

PacketCaptureClient::PacketCaptureClient() :
    m_handle(nullptr),
    m_aborting(0)
{
}

PacketCaptureClient::~PacketCaptureClient()
{
    if (!m_progress.isNull()) {
        m_aborting = 1;
        m_progress->setCancelled(true);
        if (m_loopFuture.isRunning()) {
            m_loopFuture.waitForFinished();
        }
    }
}

QSharedPointer<ImportResult> PacketCaptureClient::capturePackets(QJsonObject parameters, QSharedPointer<PluginActionProgress> progress) {
    QScopedPointer<PacketCaptureClient> client(new PacketCaptureClient());
    return client->capturePacketsImpl(parameters, progress);
}

QSharedPointer<ImportResult> PacketCaptureClient::captureLoop(PacketCaptureClient *capture)
{
    void *arg = capture;

    while (true) {
        int loopRet = pcap_dispatch(capture->m_handle, capture->m_bufferSize, &PacketCaptureClient::processPacket, static_cast<u_char*>(arg));

        if (capture->m_aborting != 0) {
            return ImportResult::error("Packet capture aborted");
        }
        else if (loopRet == PCAP_ERROR) {
            return ImportResult::error("Error during packet capture");
        }
        else if (capture->m_progress->isCancelled()) {
            return configureResult(capture);
        }

        int packets = int(capture->m_packetSizes->size());
        int bytes = int(capture->m_packetSizes->getValueCount() / 8);

        if (capture->m_maxSize > 0) {
            if (bytes >= capture->m_maxSize) {
                return configureResult(capture);
            }
            capture->m_progress->setProgress(bytes, capture->m_maxSize);
        }
        else if (capture->m_maxPackets > 0) {
            if (packets >= capture->m_maxPackets) {
                return configureResult(capture);
            }
            capture->m_progress->setProgress(packets, capture->m_maxPackets);
        }
    }
}

void PacketCaptureClient::processPacket(u_char *args, const pcap_pkthdr *header, const u_char *packet)
{
    PacketCaptureClient * capture = static_cast<PacketCaptureClient*>(static_cast<void*>(args));
    if (capture->m_progress->isCancelled()) {
        return;
    }
    capture->m_file.write(reinterpret_cast<const char*>(packet), header->caplen);
    capture->m_packetSizes->appendRange(header->caplen * 8);
}

QSharedPointer<ImportResult> PacketCaptureClient::configureResult(PacketCaptureClient *capture)
{
    if (capture->m_file.size() > 0) {
        capture->m_file.seek(0);
        auto container = BitContainer::create(&capture->m_file, capture->m_packetSizes->getValueCount());
        auto info = BitInfo::create(container->bits()->sizeInBits());
        info->setFrames(capture->m_packetSizes);
        container->setInfo(info);
        container->setName("Packet Capture");
        capture->m_file.close();
        return ImportResult::result(container, capture->m_parameters);
    }
    else {
        return ImportResult::nullResult();
    }
}

void PacketCaptureClient::extractParameters(QJsonObject parameters)
{
    m_parameters = parameters;
    m_deviceName = parameters.value("device_name").toString();
    int intAddress = parameters.value("address").toInt();
    m_address = *reinterpret_cast<quint32*>(&intAddress);
    m_maxPackets = -1;
    m_maxSize = -1;
    if (parameters.contains("max_packets")) {
        m_maxPackets = parameters.value("max_packets").toInt();
    }
    else if (parameters.contains("max_size")) {
        m_maxSize = parameters.value("max_size").toInt();
    }
    m_timeout = parameters.value("timeout").toInt();
    m_filter = "";
    if (parameters.contains("filter")) {
        m_filter = parameters.value("filter").toString();
    }
    m_bufferSize = BUFSIZ;
    if (parameters.contains("buffer_size")) {
        m_bufferSize = parameters.value("buffer_size").toInt();
    }
    m_packetSizeLimit = 65535;
    if (parameters.contains("packet_size_limit")) {
        m_packetSizeLimit = parameters.value("packet_size_limit").toInt();
    }
    m_promiscuous = parameters.value("promiscuous").toBool() ? 1 : 0;
}

QSharedPointer<ImportResult> PacketCaptureClient::capturePacketsImpl(QJsonObject parameters, QSharedPointer<PluginActionProgress> progress)
{
    m_progress = progress;
    extractParameters(parameters);

    if (m_handle != nullptr) {
        return ImportResult::error("Cannot initialize new capture while a capture loop is still running");
    }

    char errbuf[PCAP_ERRBUF_SIZE];
    m_handle = pcap_open_live(m_deviceName.toStdString().c_str(), m_packetSizeLimit, m_promiscuous, m_timeout, errbuf);
    if (m_handle == nullptr) {
        return ImportResult::error("Failed to open pcap handle:\n" + QString(errbuf));
    }

    if (!m_filter.isEmpty()) {
        struct bpf_program filter;
        if (pcap_compile(m_handle, &filter, m_filter.toStdString().c_str(), 1, m_address) == -1) {
            pcap_close(m_handle);
            m_handle = nullptr;
            return ImportResult::error("Failed to compile filter:\n" + QString(errbuf));
        }
        if (pcap_setfilter(m_handle, &filter) == -1) {
            pcap_close(m_handle);
            m_handle = nullptr;
            return ImportResult::error("Failed to apply filter:\n" + QString(errbuf));
        }
    }

    if (!m_file.open()) {
        pcap_close(m_handle);
        m_handle = nullptr;
        return ImportResult::error("Failed to open a temporary file buffer for packets");
    }

    m_file.resize(0);
    m_packetSizes = RangeSequence::createEmpty();
    m_loopFuture = QtConcurrent::run(&PacketCaptureClient::captureLoop, this);

    m_loopFuture.waitForFinished();

    pcap_close(m_handle);
    m_handle = nullptr;

    return m_loopFuture.result();
}
