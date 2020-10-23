#include "bitcontainer.h"
#include "httpexportform.h"
#include "ui_httptransceiver.h"
#include <QHttpMultiPart>
#include <QMessageBox>
#include <QThread>

HttpExportForm::HttpExportForm(QSharedPointer<ParameterDelegate> delegate) :
    ui(new Ui::HttpTransceiver),
    m_paramHelper(new ParameterHelper(delegate))
{
    ui->setupUi(this);

    ui->cb_verb->clear();
    ui->cb_verb->addItem("POST");
    ui->cb_verb->addItem("PUT");
    ui->cb_verb->setEnabled(true);
    ui->le_formDataName->setVisible(true);
    ui->lb_formDataName->setVisible(true);

    m_paramHelper->addLineEditStringParameter("url", ui->le_url);
    m_paramHelper->addLineEditStringParameter("formdataname", ui->le_formDataName);
    m_paramHelper->addComboBoxParameter("verb", ui->cb_verb, Qt::DisplayRole);
}

HttpExportForm::~HttpExportForm()
{
    delete ui;
}

QString HttpExportForm::title()
{
    return "Configure HTTP Request";
}

bool HttpExportForm::setParameters(QJsonObject parameters)
{
    return m_paramHelper->applyParametersToUi(parameters);
}

QJsonObject HttpExportForm::parameters()
{
    return m_paramHelper->getParametersFromUi();
}

bool HttpExportForm::isStandaloneDialog()
{
    return true;
}

QSharedPointer<ExportResult> HttpExportForm::exportData(QByteArray data,
                                                        QJsonObject parameters,
                                                        QSharedPointer<PluginActionProgress> progress)
{
    QString url = parameters.value("url").toString();
    QString verb = parameters.value("verb").toString();
    QString formDataName = parameters.value("formdataname").toString();

    QNetworkRequest request(url);
    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QHttpPart part;
    part.setHeader(
            QNetworkRequest::ContentDispositionHeader,
            QVariant(QString("form-data; name=\"%1\"").arg(formDataName)));
    part.setBody(data);
    multiPart->append(part);

    QNetworkAccessManager netManager;
    QNetworkReply *reply;
    if (verb == "POST") {
        reply = netManager.post(request, multiPart);
    }
    else if (verb == "PUT") {
        reply = netManager.put(request, multiPart);
    }
    else {
        return ExportResult::error(QString("Unsupported HTTP Export Verb: '%1'").arg(verb));
    }

    multiPart->setParent(reply);

    connect(reply, SIGNAL(uploadProgress(qint64, qint64)), progress.data(), SLOT(setProgress(qint64, qint64)));

    while (!reply->isFinished() && reply->error() == QNetworkReply::NoError) {
        qApp->processEvents(QEventLoop::WaitForMoreEvents | QEventLoop::ExcludeUserInputEvents, 200);
        if (progress->isCancelled()) {
            reply->abort();
            reply->deleteLater();
            return ExportResult::error("HTTP Upload Cancelled");
        }
    }

    if (reply->error() != QNetworkReply::NoError) {
        QString error = reply->errorString();
        reply->abort();
        reply->deleteLater();
        return ExportResult::error("HTTP Upload error: " + error);
    }

    reply->deleteLater();
    return ExportResult::result(parameters);
}

void HttpExportForm::on_pb_send_clicked()
{
    emit accepted();
}
