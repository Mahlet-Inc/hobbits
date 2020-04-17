#include "tcpreceiver.h"
#include "ui_tcpreceiver.h"
#include <QTcpSocket>

TcpReceiver::TcpReceiver(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TcpReceiver),
    m_server(new QTcpServer(this)),
    m_socket(nullptr)
{
    ui->setupUi(this);
    connect(m_server, &QTcpServer::newConnection, this, &TcpReceiver::manageConnection);
    m_downloadFile.open();

    setWindowTitle("Import via TCP");

    ui->pb_stop->setEnabled(false);
}

TcpReceiver::~TcpReceiver()
{
    delete ui;
}

QTemporaryFile* TcpReceiver::getDownloadedData()
{
    m_downloadFile.seek(0);
    return &m_downloadFile;
}

int TcpReceiver::getPort()
{
    return ui->sb_port->value();
}

void TcpReceiver::setPort(int port)
{
    ui->sb_port->setValue(port);
}

QString TcpReceiver::getError()
{
    if (m_socket) {
        return m_socket->errorString();
    }
    return m_server->errorString();
}

void TcpReceiver::startListening()
{
    ui->pb_start->setText("Listening...");
    ui->pb_start->setEnabled(false);
    ui->pb_stop->setEnabled(true);
    ui->sb_port->setEnabled(false);
    m_downloadFile.resize(0);
    ui->lb_progress->setText(QString("Waiting for connection..."));
    m_server->listen(QHostAddress::Any, quint16(ui->sb_port->value()));
}

void TcpReceiver::stopListening()
{
    ui->pb_start->setText("Begin Listening");
    ui->lb_progress->setText(QString("Server Stopped, %1 KBs Received").arg(double(m_downloadFile.size())/1000.0, 1, 'g', 3));
    ui->pb_start->setEnabled(true);
    ui->pb_stop->setEnabled(false);
    ui->sb_port->setEnabled(true);
    if (m_socket) {
        m_socket->close();
    }
    m_server->close();
    if (m_downloadFile.size() > 0) {
        accept();
    }
}

void TcpReceiver::manageConnection()
{
    if (!m_server->hasPendingConnections()) {
        return;
    }
    if (m_socket) {
        disconnect(m_socket, &QTcpSocket::readyRead, this, &TcpReceiver::socketRead);
        disconnect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));
    }
    m_socket = m_server->nextPendingConnection();
    connect(m_socket, &QTcpSocket::readyRead, this, &TcpReceiver::socketRead);
    connect(m_socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(socketError(QAbstractSocket::SocketError)));
    ui->lb_progress->setText(QString("Connection received, waiting for data..."));
}

void TcpReceiver::socketError(QAbstractSocket::SocketError)
{
    stopListening();
    if (m_downloadFile.size() > 0) {
        accept();
    }
    else {
        reject();
    }
}

void TcpReceiver::socketRead()
{
    while (m_socket->bytesAvailable() > 0) {
        m_downloadFile.write(m_socket->read(10000));
    }
    ui->lb_progress->setText(QString("%1 KBs Received").arg(double(m_downloadFile.size())/1000.0, 1, 'g', 3));
}

void TcpReceiver::on_pb_start_pressed()
{
    startListening();
}

void TcpReceiver::on_pb_stop_pressed()
{
    stopListening();
}
