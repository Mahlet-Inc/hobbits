#ifndef CAPTUREDIALOG_H
#define CAPTUREDIALOG_H

#include <QDialog>
#include <QTemporaryFile>
#include <pcap/pcap.h>
#include "bitcontainer.h"

namespace Ui {
class CaptureDialog;
}

class CaptureDialog : public QDialog
{
    Q_OBJECT

public:
    explicit CaptureDialog(QWidget *parent = nullptr);
    ~CaptureDialog();

    static void captureLoop(CaptureDialog *capture);
    static void processPacket(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);

    QSharedPointer<BitContainer> container() const;

private slots:
    void populateDeviceBox();
    void checkCurrDevice();
    void endLoop();
    void finalizeCapture(QString error = QString());
    void endCapture(QString error = QString());

    void setProgress(int packets, int bytes);

    void on_pb_beginCapture_clicked();

    void on_pb_stopCapture_clicked();

private:
    Ui::CaptureDialog *ui;

    pcap_t *m_handle;
    QTemporaryFile m_file;
    QSharedPointer<BitContainer> m_container;
    QSharedPointer<RangeSequence> m_packetSizes;
};

#endif // CAPTUREDIALOG_H
