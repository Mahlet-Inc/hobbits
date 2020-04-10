#ifndef TCPSENDER_H
#define TCPSENDER_H

#include <QDialog>
#include <QTcpSocket>
#include "bitarray.h"

namespace Ui {
class TcpSender;
}

class TcpSender : public QDialog
{
    Q_OBJECT

public:
    explicit TcpSender(QSharedPointer<const BitArray> bits, QWidget *parent = nullptr);
    ~TcpSender();

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
    Ui::TcpSender *ui;
    QSharedPointer<const BitArray> m_bits;
    QSharedPointer<QTcpSocket> m_socket;
    qint64 m_bytesWritten;
};

#endif // TCPSENDER_H
