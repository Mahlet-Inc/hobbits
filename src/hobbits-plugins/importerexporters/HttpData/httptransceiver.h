#ifndef HTTPTRANSCEIVER_H
#define HTTPTRANSCEIVER_H

#include <QDialog>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTemporaryFile>

namespace Ui
{
class HttpTransceiver;
}

class HttpTransceiver : public QDialog
{
    Q_OBJECT

public:
    explicit HttpTransceiver(QWidget *parent = nullptr);

    ~HttpTransceiver();

    QTemporaryFile* getDownloadedData();

public slots:
    void setUploadMode(QByteArray data);
    void setDownloadMode();

private slots:
    void on_pb_send_clicked();
    void replyReceived(QNetworkReply*);

    void progressReceived(qint64, qint64);
    void handleError(QNetworkReply::NetworkError);

    void interruptDownload();

private:
    Ui::HttpTransceiver *ui;
    QNetworkAccessManager *m_netManager;
    QTemporaryFile *m_downloadFile;
    QByteArray m_uploadData;
    QNetworkReply *m_reply;
};

#endif // HTTPTRANSCEIVER_H
