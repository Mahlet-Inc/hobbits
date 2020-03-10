#include "bitcontainer.h"
#include "httptransceiver.h"
#include "ui_httptransceiver.h"
#include <QHttpMultiPart>
#include <QMessageBox>

HttpTransceiver::HttpTransceiver(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HttpTransceiver),
    m_netManager(new QNetworkAccessManager(this)),
    m_downloadFile(new QTemporaryFile),
    m_reply(nullptr)
{
    ui->setupUi(this);

    this->setWindowTitle("HTTP Request");
    m_downloadFile->open();

    setDownloadMode();
    connect(
            m_netManager,
            &QNetworkAccessManager::finished,
            this,
            &HttpTransceiver::replyReceived);

    connect(ui->pb_interrupt, &QPushButton::clicked, this, &HttpTransceiver::interruptDownload);
}

HttpTransceiver::~HttpTransceiver()
{
    delete ui;
}

QTemporaryFile* HttpTransceiver::getDownloadedData()
{
    m_downloadFile->seek(0);
    return m_downloadFile;
}

void HttpTransceiver::setUploadMode(QByteArray data)
{
    m_uploadData = data;
    ui->cb_verb->clear();
    ui->cb_verb->addItem("POST");
    ui->cb_verb->addItem("PUT");
    ui->cb_verb->setEnabled(true);
    ui->le_formDataName->setVisible(true);
    ui->lb_formDataName->setVisible(true);
    ui->pr_request->setValue(0);
}

void HttpTransceiver::setDownloadMode()
{
    ui->cb_verb->clear();
    ui->cb_verb->addItem("GET");
    ui->cb_verb->setEnabled(false);
    ui->le_formDataName->setVisible(false);
    ui->lb_formDataName->setVisible(false);
    ui->pr_request->setValue(0);
}

void HttpTransceiver::on_pb_send_clicked()
{
    ui->pb_send->setEnabled(false);
    ui->cb_verb->setEnabled(false);
    QNetworkRequest request(QUrl(ui->le_url->text()));

    QNetworkReply *reply;

    if (ui->cb_verb->currentText() == "GET") {
        ui->pb_interrupt->setEnabled(true);
        m_downloadFile->resize(0);
        reply = m_netManager->get(request);

        connect(reply, &QNetworkReply::downloadProgress, this, &HttpTransceiver::progressReceived);
    }
    else {
        QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

        QHttpPart part;
        part.setHeader(
                QNetworkRequest::ContentDispositionHeader,
                QVariant(QString("form-data; name=\"%1\"").arg(ui->le_formDataName->text())));
        part.setBody(m_uploadData);

        multiPart->append(part);

        if (ui->cb_verb->currentText() == "POST") {
            reply = m_netManager->post(request, multiPart);
        }
        else if (ui->cb_verb->currentText() == "PUT") {
            reply = m_netManager->put(request, multiPart);
        }
        else {
            QMessageBox msg(this);
            msg.setWindowTitle("Unsupported HTTP Verb");
            msg.setText(QString("The selected HTTP Verb '%1' is unsupported").arg(ui->cb_verb->currentText()));
            msg.setDefaultButton(QMessageBox::Ok);
            msg.exec();
            return;
        }

        multiPart->setParent(reply);

        connect(reply, &QNetworkReply::uploadProgress, this, &HttpTransceiver::progressReceived);
    }

    if (m_reply) {
        m_reply->deleteLater();
    }
    m_reply = reply;

    connect(
            reply,
            QOverload<QNetworkReply::NetworkError>::of(&QNetworkReply::error),
            this,
            &HttpTransceiver::handleError);
}

void HttpTransceiver::interruptDownload()
{
    replyReceived(m_reply);
    m_reply->abort();
}

void HttpTransceiver::progressReceived(qint64 progress, qint64 total)
{
    ui->pr_request->setValue(int(double(progress) / double(total) * 100.0));
    if (ui->cb_verb->currentText() == "GET") {
        while (m_reply->bytesAvailable()) {
            m_downloadFile->write(m_reply->read(10 * 1000 * 1000));
        }
    }
}

void HttpTransceiver::replyReceived(QNetworkReply *reply)
{
    m_uploadData.clear();

    if (reply->error() == QNetworkReply::NoError) {
        this->accept();
        ui->pr_request->setValue(0);
        ui->le_url->clear();
        ui->le_formDataName->clear();
    }

    ui->pb_send->setEnabled(true);
    ui->cb_verb->setEnabled(true);
    ui->pb_interrupt->setEnabled(false);
}

void HttpTransceiver::handleError(QNetworkReply::NetworkError error)
{
    Q_UNUSED(error)
    QMessageBox msg(this);
    msg.setWindowTitle("HTTP Error");
    msg.setText(QString("Error encountered in HTTP request: '%1'").arg(m_reply->errorString()));
    msg.setDefaultButton(QMessageBox::Ok);
    msg.exec();
}
