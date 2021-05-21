#include "tcpreceiver.h"
#include "ui_tcpreceiver.h"
#include <QTcpSocket>

TcpReceiver::TcpReceiver(QSharedPointer<ParameterDelegate> delegate) :
    ui(new Ui::TcpReceiver),
    m_paramHelper(new ParameterHelper(delegate))
{
    ui->setupUi(this);

    m_paramHelper->addSpinBoxIntParameter("port", ui->sb_port);
    m_paramHelper->addSpinBoxIntParameter("max_kb", ui->sb_maxLengthKb);
    m_paramHelper->addSpinBoxIntParameter("timeout", ui->sb_timeout);
}

TcpReceiver::~TcpReceiver()
{
    delete ui;
}

QString TcpReceiver::title()
{
    return "Configure TCP Receiver";
}

bool TcpReceiver::setParameters(const Parameters &parameters)
{
    return m_paramHelper->applyParametersToUi(parameters);
}

Parameters TcpReceiver::parameters()
{
    return m_paramHelper->getParametersFromUi();
}

bool TcpReceiver::isStandaloneDialog()
{
    return true;
}

QSharedPointer<ImportResult> TcpReceiver::importData(const Parameters &parameters, QSharedPointer<PluginActionProgress> progress)
{
    int port = parameters.value("port").toInt();
    qint64 maxBytes = qint64(parameters.value("max_kb").toInt()) * 1000ll;
    int timeout = parameters.value("timeout").toInt() * 1000;

    QTemporaryFile downloadBuffer;
    if (!downloadBuffer.open()) {
        return ImportResult::error("Failed to open buffer file for downloaded data");
    }

    QScopedPointer<QTcpServer> server(new QTcpServer());
    server->listen(QHostAddress::Any, quint16(port));

    if (!server->waitForNewConnection(timeout)) {
        return ImportResult::error("No TCP connection was made before the configured timeout.");
    }

    auto socket = server->nextPendingConnection();
    while (true) {
        if (downloadBuffer.size() >= maxBytes
                || progress->isCancelled()
                || !socket->isOpen()
                || !socket->waitForReadyRead(timeout)) {
            break;
        }

        while (socket->bytesAvailable() > 0) {
            downloadBuffer.write(socket->read(10 * 1000 * 1000));
            progress->setProgress(downloadBuffer.size(), maxBytes);
        }
    }

    socket->close();

    if (downloadBuffer.size() < 1) {
        if (!socket->errorString().isEmpty()) {
            return ImportResult::error("TCP Socket error: " + socket->errorString());
        }
        return ImportResult::error("No data was received on TCP connection.");
    }

    downloadBuffer.seek(0);
    return ImportResult::result(BitContainer::create(&downloadBuffer), parameters);
}

void TcpReceiver::on_pb_start_pressed()
{
    emit accepted();
}
