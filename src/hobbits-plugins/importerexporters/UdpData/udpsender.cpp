#include "udpsender.h"
#include "ui_udpsender.h"

UdpSender::UdpSender(QSharedPointer<ParameterDelegate> delegate) :
    ui(new Ui::UdpSender),
    m_paramHelper(new ParameterHelper(delegate))
{
    ui->setupUi(this);

    m_paramHelper->addLineEditStringParameter("host", ui->le_host);
    m_paramHelper->addSpinBoxIntParameter("port", ui->sb_port);
}

UdpSender::~UdpSender()
{
    delete ui;
}

QString UdpSender::title()
{
    return "Configure UDP Export Connnection";
}

bool UdpSender::setParameters(const Parameters &parameters)
{
    return m_paramHelper->applyParametersToUi(parameters);
}

Parameters UdpSender::parameters()
{
    return m_paramHelper->getParametersFromUi();
}

bool UdpSender::isStandaloneDialog()
{
    return true;
}

#define UDPWRITEBUFFSIZE 512
QSharedPointer<ExportResult> UdpSender::exportData(QSharedPointer<const BitArray> bits, const Parameters &parameters, QSharedPointer<PluginActionProgress> progress)
{
    QHostAddress host(parameters.value("host").toString());
    quint16 port = quint16(parameters.value("port").toInt());

    auto socket = new QUdpSocket();

    qint64 bytesWritten = 0;
    char writeBuffer[UDPWRITEBUFFSIZE];

    while (bits->sizeInBytes() > bytesWritten) {
        qint64 size = bits->readBytes(writeBuffer, bytesWritten, UDPWRITEBUFFSIZE);
        qint64 written = socket->writeDatagram(writeBuffer, size, host, port);
        if (written < 1) {
            socket->close();
            socket->deleteLater();
            return ExportResult::error("Failed to write datagram");
        }
        bytesWritten += written;
        progress->setProgress(bytesWritten, bits->sizeInBytes());
    }

    socket->close();
    socket->deleteLater();
    return ExportResult::result(parameters);
}

void UdpSender::on_pb_send_pressed()
{
    emit accepted();
}

