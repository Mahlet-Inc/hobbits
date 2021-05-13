#include "udpreceiver.h"
#include "ui_udpreceiver.h"
#include <QNetworkDatagram>
#include <QTimer>

UdpReceiver::UdpReceiver(QSharedPointer<ParameterDelegate> delegate) :
    ui(new Ui::UdpReceiver),
    m_paramHelper(new ParameterHelper(delegate))
{
    ui->setupUi(this);

    m_paramHelper->addSpinBoxIntParameter("port", ui->sb_port);
    m_paramHelper->addSpinBoxIntParameter("max_kb", ui->sb_limitKb);
    m_paramHelper->addSpinBoxIntParameter("timeout", ui->sb_timeout);
}

UdpReceiver::~UdpReceiver()
{
    delete ui;
}

QString UdpReceiver::title()
{
    return "Configure UDP Receiver";
}

bool UdpReceiver::setParameters(const Parameters &parameters)
{
    return m_paramHelper->applyParametersToUi(parameters);
}

Parameters UdpReceiver::parameters()
{
    return m_paramHelper->getParametersFromUi();
}

bool UdpReceiver::isStandaloneDialog()
{
    return true;
}

QSharedPointer<ImportResult> UdpReceiver::importData(const Parameters &parameters, QSharedPointer<PluginActionProgress> progress)
{
    int port = parameters.value("port").toInt();
    qint64 maxBytes = qint64(parameters.value("max_kb").toInt()) * 1000ll;
    int timeout = parameters.value("timeout").toInt() * 1000;

    QTemporaryFile downloadBuffer;
    if (!downloadBuffer.open()) {
        return ImportResult::error("Failed to open buffer file for downloaded data");
    }

    auto socket = new QUdpSocket();
    socket->bind(QHostAddress::LocalHost, quint16(port));

    while (true) {
        if (downloadBuffer.size() >= maxBytes
                || progress->isCancelled()
                || !socket->waitForReadyRead(timeout)) {
            break;
        }

        while (socket->hasPendingDatagrams()) {
            auto datagram = socket->receiveDatagram();
            downloadBuffer.write(datagram.data());
            progress->setProgress(downloadBuffer.size(), maxBytes);
        }
    }

    socket->close();
    QString socketErr = socket->errorString();
    socket->deleteLater();

    if (downloadBuffer.size() < 1) {
        if (!socketErr.isEmpty()) {
            return ImportResult::error("UDP Socket error: " + socketErr);
        }
        return ImportResult::error("No data was received on UDP connection.");
    }

    downloadBuffer.seek(0);
    return ImportResult::result(BitContainer::create(&downloadBuffer), parameters);
}

void UdpReceiver::on_pb_start_pressed()
{
    emit accepted();
}
