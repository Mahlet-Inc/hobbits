#include "bitcontainer.h"
#include "httpimportform.h"
#include "ui_httptransceiver.h"
#include <QHttpMultiPart>
#include <QMessageBox>
#include <QTemporaryFile>
#include <QThread>

HttpImportForm::HttpImportForm(QSharedPointer<ParameterDelegate> delegate) :
    ui(new Ui::HttpTransceiver),
    m_paramHelper(new ParameterHelper(delegate))
{
    ui->setupUi(this);

    ui->cb_verb->clear();
    ui->cb_verb->addItem("GET");
    ui->cb_verb->setEnabled(false);
    ui->le_formDataName->setVisible(false);
    ui->lb_formDataName->setVisible(false);

    m_paramHelper->addLineEditStringParameter("url", ui->le_url);
    m_paramHelper->addLineEditStringParameter("formdataname", ui->le_formDataName);
    m_paramHelper->addComboBoxParameter("verb", ui->cb_verb, Qt::DisplayRole);
}

HttpImportForm::~HttpImportForm()
{
    delete ui;
}

QString HttpImportForm::title()
{
    return "Configure HTTP Request";
}

bool HttpImportForm::setParameters(QJsonObject parameters)
{
    return m_paramHelper->applyParametersToUi(parameters);
}

QJsonObject HttpImportForm::parameters()
{
    return m_paramHelper->getParametersFromUi();
}

bool HttpImportForm::isStandaloneDialog()
{
    return true;
}

QSharedPointer<ImportResult> HttpImportForm::importData(QJsonObject parameters, QSharedPointer<PluginActionProgress> progress)
{
    QString url = parameters.value("url").toString();
    QString verb = parameters.value("verb").toString();

    if (verb != "GET") {
        return ImportResult::error(QString("Unsupported HTTP Import Verb: '%1'").arg(verb));
    }

     QNetworkRequest request(url);
     QNetworkAccessManager netManager;
     QNetworkReply *reply = netManager.get(request);

     QString err = reply->errorString();

     connect(reply, SIGNAL(downloadProgress(qint64, qint64)), progress.data(), SLOT(setProgress(qint64, qint64)));

     QTemporaryFile downloadBuffer;
     if (!downloadBuffer.open()) {
         reply->abort();
         reply->deleteLater();
         return ImportResult::error("Failed to open buffer file for downloaded data");
     }
     while (!reply->isFinished()) {
         qApp->processEvents(QEventLoop::WaitForMoreEvents | QEventLoop::ExcludeUserInputEvents, 200);
         err = reply->errorString();
         if (reply->error() != QNetworkReply::NoError) {
             reply->abort();
             reply->deleteLater();
             return ImportResult::error("HTTP Download error: " + reply->errorString());
         }
         if (progress->isCancelled()) {
             reply->abort();
             reply->deleteLater();
             // Treat cancel with data as partial download, not failure
             if (downloadBuffer.size() > 0) {
                 downloadBuffer.seek(0);
                 return ImportResult::result(BitContainer::create(&downloadBuffer), parameters);
             }
             return ImportResult::error("HTTP Download Cancelled");
         }
         while (reply->bytesAvailable()) {
             downloadBuffer.write(reply->read(10 * 1000 * 1000));
         }
     }

     if (reply->error() != QNetworkReply::NoError) {
         QString error = reply->errorString();
         reply->deleteLater();
         return ImportResult::error("HTTP Download error: " + error);
     }

     reply->deleteLater();
     downloadBuffer.seek(0);
     return ImportResult::result(BitContainer::create(&downloadBuffer), parameters);
}

void HttpImportForm::on_pb_send_clicked()
{
    emit accepted();
}
