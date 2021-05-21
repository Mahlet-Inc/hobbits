#include "tcpsender.h"
#include "ui_tcpsender.h"

TcpSender::TcpSender(QSharedPointer<ParameterDelegate> delegate) :
    ui(new Ui::TcpSender),
    m_paramHelper(new ParameterHelper(delegate))
{
    ui->setupUi(this);

    m_paramHelper->addLineEditStringParameter("host", ui->le_host);
    m_paramHelper->addSpinBoxIntParameter("port", ui->sb_port);
    m_paramHelper->addSpinBoxIntParameter("timeout", ui->sb_timeout);
}

TcpSender::~TcpSender()
{
    delete ui;
}

QString TcpSender::title()
{
    return "Configure TCP Export Connnection";
}

bool TcpSender::setParameters(const Parameters &parameters)
{
    return m_paramHelper->applyParametersToUi(parameters);
}

Parameters TcpSender::parameters()
{
    return m_paramHelper->getParametersFromUi();
}

bool TcpSender::isStandaloneDialog()
{
    return true;
}

#define TCPWRITEBUFFSIZE 8192
QSharedPointer<ExportResult> TcpSender::exportData(QSharedPointer<const BitArray> bits, const Parameters &parameters, QSharedPointer<PluginActionProgress> progress)
{
    QString host = parameters.value("host").toString();
    int port = parameters.value("port").toInt();
    int timeout = parameters.value("timeout").toInt() * 1000;

    auto socket = new QTcpSocket();
    socket->connectToHost(host, quint16(port), QIODevice::WriteOnly);

    qint64 bytesWritten = 0;
    char writeBuffer[TCPWRITEBUFFSIZE];

    while (bits->sizeInBytes() > bytesWritten) {
        qint64 size = bits->readBytes(writeBuffer, bytesWritten, TCPWRITEBUFFSIZE);
        bytesWritten += socket->write(writeBuffer, size);
        if (!socket->waitForBytesWritten(timeout)) {
            socket->close();
            socket->deleteLater();
            return ExportResult::error("Failed to write bits before timeout");
        }
        progress->setProgress(bytesWritten, bits->sizeInBytes());
    }

    socket->close();
    socket->deleteLater();
    return ExportResult::result(parameters);
}

void TcpSender::on_pb_send_pressed()
{
    emit accepted();
}
