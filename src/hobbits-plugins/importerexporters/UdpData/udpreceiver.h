#ifndef UDPRECEIVER_H
#define UDPRECEIVER_H

#include <QDialog>
#include <QUdpSocket>
#include <QTemporaryFile>

namespace Ui {
class UdpReceiver;
}

class UdpReceiver : public QDialog
{
    Q_OBJECT

public:
    explicit UdpReceiver(QWidget *parent = nullptr);
    ~UdpReceiver();

    QTemporaryFile* getDownloadedData();
    int getPort();
    void setPort(int);
    int getLimit();
    void setLimit(int bytes);
    QString getError();

public slots:
    void startListening();
    void stopListening();

private slots:
    void socketError(QAbstractSocket::SocketError);
    void socketRead();

    void on_pb_start_pressed();
    void on_pb_stop_pressed();

private:
    Ui::UdpReceiver *ui;
    QUdpSocket* m_socket;
    QTemporaryFile m_downloadFile;
};

#endif // UDPRECEIVER_H
