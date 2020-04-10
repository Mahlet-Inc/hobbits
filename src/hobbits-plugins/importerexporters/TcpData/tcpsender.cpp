#include "tcpsender.h"
#include "ui_tcpsender.h"

TcpSender::TcpSender(QSharedPointer<const BitArray> bits, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::TcpSender),
    m_bits(bits),
    m_socket(new QTcpSocket()),
    m_bytesWritten(0)
{
    ui->setupUi(this);
    ui->pb_cancel->setEnabled(false);

    connect(m_socket.data(), &QTcpSocket::bytesWritten, this, &TcpSender::writeNextChunk);
}

TcpSender::~TcpSender()
{
    delete ui;
}


void TcpSender::setHost(QString host)
{
    ui->le_host->setText(host);
}

QString TcpSender::getHost() const
{
    return ui->le_host->text();
}

void TcpSender::setPort(int port)
{
    ui->sb_port->setValue(port);
}

int TcpSender::getPort() const
{
    return ui->sb_port->value();
}

qint64 TcpSender::getBytesWritten()
{
    return m_bytesWritten;
}

void TcpSender::sendData()
{
    ui->pb_send->setText("Sending...");
    ui->pb_send->setEnabled(false);
    ui->pb_cancel->setEnabled(true);
    ui->le_host->setEnabled(false);
    ui->sb_port->setEnabled(false);
    m_socket->connectToHost(ui->le_host->text(), quint16(ui->sb_port->value()), QIODevice::WriteOnly);
    writeNextChunk(0);
}

void TcpSender::writeNextChunk(qint64 lastChunkSize)
{
    if (m_bits->sizeInBytes() < 1) {
        reject();
    }
    m_bytesWritten += lastChunkSize;
    ui->pg_sent->setValue(int(100.0 * double(m_bytesWritten) / double(m_bits->sizeInBytes())));

    if (m_bits->sizeInBytes() > m_bytesWritten) {
        int bufferSize = 1024;
        char *buffer = new char[bufferSize];
        qint64 size = m_bits->readBytes(buffer, m_bytesWritten, bufferSize);
        m_socket->write(buffer, size);
    }
    else {
        cancelSendData();
        accept();
    }
}

void TcpSender::socketError(QAbstractSocket::SocketError)
{
    cancelSendData();
    reject();
}

void TcpSender::cancelSendData()
{
    m_socket->close();
    ui->pb_send->setText("Send");
    ui->pb_send->setEnabled(true);
    ui->pb_cancel->setEnabled(false);
    ui->le_host->setEnabled(true);
    ui->sb_port->setEnabled(true);
}

void TcpSender::on_pb_send_pressed()
{
    sendData();
}

void TcpSender::on_pb_cancel_pressed()
{
    cancelSendData();
    accept();
}
