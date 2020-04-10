#ifndef TCPRECEIVER_H
#define TCPRECEIVER_H

#include <QDialog>
#include <QTcpServer>
#include <QTemporaryFile>

namespace Ui {
class TcpReceiver;
}

class TcpReceiver : public QDialog
{
    Q_OBJECT

public:
    explicit TcpReceiver(QWidget *parent = nullptr);
    ~TcpReceiver();

    QTemporaryFile* getDownloadedData();
    int getPort();
    void setPort(int);
    QString getError();

public slots:
    void startListening();
    void stopListening();

private slots:
    void manageConnection();
    void socketError(QAbstractSocket::SocketError);
    void socketRead();

    void on_pb_start_pressed();
    void on_pb_stop_pressed();

private:
    Ui::TcpReceiver *ui;
    QTcpServer *m_server;
    QTcpSocket* m_socket;
    QTemporaryFile m_downloadFile;
};

#endif // TCPRECEIVER_H
