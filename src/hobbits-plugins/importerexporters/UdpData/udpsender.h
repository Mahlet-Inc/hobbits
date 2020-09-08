#ifndef UDPSENDER_H
#define UDPSENDER_H

#include <QDialog>
#include <QUdpSocket>
#include "bitarray.h"

namespace Ui {
class UdpSender;
}

class UdpSender : public QDialog
{
    Q_OBJECT

public:
    explicit UdpSender(QSharedPointer<const BitArray> bits, QWidget *parent = nullptr);
    ~UdpSender();

    void setHost(QString host);
    QString getHost() const;
    void setPort(int port);
    int getPort() const;
    qint64 getBytesWritten();

public slots:
    void sendData();
    void cancelSendData();
    void writeNextChunk(qint64 lastChunkSize);
    void socketError(QAbstractSocket::SocketError);

private slots:
    void on_pb_send_pressed();

    void on_pb_cancel_pressed();

private:
    Ui::UdpSender *ui;
    QSharedPointer<const BitArray> m_bits;
    QSharedPointer<QUdpSocket> m_socket;
    qint64 m_bytesWritten;

};

#endif // UDPSENDER_H
