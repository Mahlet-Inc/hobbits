#ifndef CAPTUREDIALOG_H
#define CAPTUREDIALOG_H

#include <QTemporaryFile>
#include <pcap/pcap.h>
#include "bitcontainer.h"
#include <QFuture>
#include "importresult.h"
#include "pluginactionprogress.h"

class PacketCaptureClient : public QObject
{
    Q_OBJECT

public:
    ~PacketCaptureClient();
    static QSharedPointer<ImportResult> capturePackets(const Parameters &parameters, QSharedPointer<PluginActionProgress> progress);

private:
    explicit PacketCaptureClient();

    static QSharedPointer<ImportResult> captureLoop(PacketCaptureClient *capture);
    static void processPacket(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);
    static QSharedPointer<ImportResult> configureResult(PacketCaptureClient *capture);

    QSharedPointer<BitContainer> container() const;

    QSharedPointer<ImportResult> capturePacketsImpl(const Parameters &parameters, QSharedPointer<PluginActionProgress> progress);

private slots:

signals:
    void finishedWithCapture();

private:
    void extractParameters(const Parameters &parameters);
    pcap_t *m_handle;
    QTemporaryFile m_file;
    QSharedPointer<RangeSequence> m_packetSizes;
    QFuture<QSharedPointer<ImportResult>> m_loopFuture;

    QSharedPointer<PluginActionProgress> m_progress;
    QAtomicInt m_aborting;

    Parameters m_parameters;
    QString m_deviceName;
    bpf_u_int32 m_address;
    int m_maxPackets;
    int m_maxSize;
    int m_timeout;
    QString m_filter;
    int m_bufferSize;
    int m_packetSizeLimit;
    int m_promiscuous;

};

#endif // CAPTUREDIALOG_H
