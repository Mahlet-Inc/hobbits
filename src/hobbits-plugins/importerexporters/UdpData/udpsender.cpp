#include "udpsender.h"
#include "ui_udpsender.h"

UdpSender::UdpSender(QSharedPointer<const BitArray> bits, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::UdpSender),
    m_bits(bits),
    m_socket(new QUdpSocket()),
    m_bytesWritten(0)
{
    ui->setupUi(this);
    ui->pb_cancel->setEnabled(false);

    connect(m_socket.data(), &QUdpSocket::bytesWritten, this, &UdpSender::writeNextChunk);

    setWindowTitle("Export via UDP");
}

UdpSender::~UdpSender()
{
    delete ui;
}

void UdpSender::setHost(QString host)
{
    ui->le_host->setText(host);
}

QString UdpSender::getHost() const
{
    return ui->le_host->text();
}

void UdpSender::setPort(int port)
{
    ui->sb_port->setValue(port);
}

int UdpSender::getPort() const
{
    return ui->sb_port->value();
}

qint64 UdpSender::getBytesWritten()
{
    return m_bytesWritten;
}

void UdpSender::sendData()
{
    ui->pb_send->setText("Sending...");
    ui->pb_send->setEnabled(false);
    ui->pb_cancel->setEnabled(true);
    ui->le_host->setEnabled(false);
    ui->sb_port->setEnabled(false);
    writeNextChunk(0);
}

void UdpSender::writeNextChunk(qint64 lastChunkSize)
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
        m_socket->writeDatagram(buffer, size, QHostAddress(ui->le_host->text()), quint16(ui->sb_port->value()));
    }
    else {
        cancelSendData();
        accept();
    }
}

void UdpSender::socketError(QAbstractSocket::SocketError)
{
    cancelSendData();
    reject();
}

void UdpSender::cancelSendData()
{
    m_socket->close();
    ui->pb_send->setText("Send");
    ui->pb_send->setEnabled(true);
    ui->pb_cancel->setEnabled(false);
    ui->le_host->setEnabled(true);
    ui->sb_port->setEnabled(true);
}

void UdpSender::on_pb_send_pressed()
{
    sendData();
}

void UdpSender::on_pb_cancel_pressed()
{
    cancelSendData();
    accept();
}

