#include "udpreceiver.h"
#include "ui_udpreceiver.h"
#include <QNetworkDatagram>
#include <QTimer>

UdpReceiver::UdpReceiver(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UdpReceiver),
    m_socket(nullptr)
{
    ui->setupUi(this);
    m_downloadFile.open();

    setWindowTitle("Import via UDP");

    ui->pb_stop->setEnabled(false);

}

UdpReceiver::~UdpReceiver()
{
    delete ui;
}

QTemporaryFile* UdpReceiver::getDownloadedData()
{
    m_downloadFile.seek(0);
    return &m_downloadFile;
}

int UdpReceiver::getPort()
{
    return ui->sb_port->value();
}

void UdpReceiver::setPort(int port)
{
    ui->sb_port->setValue(port);
}

int UdpReceiver::getLimit()
{
    if (ui->ck_limit->isChecked()) {
        return ui->sb_limit->value() * 1000;
    }
    else {
        return -1;
    }
}

void UdpReceiver::setLimit(int bytes)
{
    if (bytes > 0) {
        ui->ck_limit->setChecked(true);
        ui->sb_limit->setValue(bytes / 1000);
    }
    else {
        ui->ck_limit->setChecked(false);
    }
}

QString UdpReceiver::getError()
{
    if (m_socket) {
        return m_socket->errorString();
    }
    return QString();
}

void UdpReceiver::startListening()
{
    ui->pb_start->setText("Listening...");
    ui->pb_start->setEnabled(false);
    ui->pb_stop->setEnabled(true);
    ui->sb_port->setEnabled(false);
    m_downloadFile.resize(0);
    ui->lb_progress->setText(QString("Waiting for data..."));
    if (m_socket) {
        delete m_socket;
    }
    m_socket = new QUdpSocket(this);

    connect(m_socket, &QUdpSocket::readyRead, this, &UdpReceiver::socketRead);
    connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));

    m_socket->bind(QHostAddress::LocalHost, quint16(getPort()));
}

void UdpReceiver::stopListening()
{
    ui->pb_start->setText("Begin Listening");
    ui->lb_progress->setText(QString("Server Stopped, %1 KBs Received").arg(double(m_downloadFile.size())/1000.0, 1, 'f', 3));
    ui->pb_start->setEnabled(true);
    ui->pb_stop->setEnabled(false);
    ui->sb_port->setEnabled(true);
    if (m_socket) {
        m_socket->close();
        delete m_socket;
        m_socket = nullptr;
    }
    if (m_downloadFile.size() > 0) {
        accept();
    }
}

void UdpReceiver::socketError(QAbstractSocket::SocketError)
{
    stopListening();
    if (m_downloadFile.size() > 0) {
        accept();
    }
    else {
        reject();
    }
}

void UdpReceiver::socketRead()
{
    while (m_socket->hasPendingDatagrams()) {
        auto datagram = m_socket->receiveDatagram();
        m_downloadFile.write(datagram.data());
    }
    ui->lb_progress->setText(QString("%1 KBs Received").arg(double(m_downloadFile.size())/1000.0, 1, 'f', 3));

    if (getLimit() > 0 && m_downloadFile.size() >= getLimit()) {
        disconnect(m_socket, &QUdpSocket::readyRead, this, &UdpReceiver::socketRead);
        QTimer::singleShot(10, this, &UdpReceiver::stopListening);
    }
}

void UdpReceiver::on_pb_start_pressed()
{
    startListening();
}

void UdpReceiver::on_pb_stop_pressed()
{
    stopListening();
}
