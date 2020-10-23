#ifndef HTTPIMPORTFORM_H
#define HTTPIMPORTFORM_H

#include "abstractparametereditor.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QTemporaryFile>
#include "parameterdelegate.h"
#include "parameterhelper.h"
#include "importresult.h"

namespace Ui
{
class HttpTransceiver;
}

class HttpImportForm : public AbstractParameterEditor
{
    Q_OBJECT

public:
    explicit HttpImportForm(QSharedPointer<ParameterDelegate> delegate);
    ~HttpImportForm() override;

    QString title() override;
    bool setParameters(QJsonObject parameters) override;
    QJsonObject parameters() override;
    bool isStandaloneDialog() override;

    static QSharedPointer<ImportResult> importData(QJsonObject parameters, QSharedPointer<PluginActionProgress> progress);

private slots:
    void on_pb_send_clicked();

private:
    Ui::HttpTransceiver *ui;
    QSharedPointer<ParameterHelper> m_paramHelper;
};

#endif // HTTPIMPORTFORM_H
