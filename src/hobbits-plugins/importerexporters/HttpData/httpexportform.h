#ifndef HTTPEXPORTFORM_H
#define HTTPEXPORTFORM_H

#include "abstractparametereditor.h"
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include "parameterdelegate.h"
#include "parameterhelper.h"
#include "exportresult.h"

namespace Ui
{
class HttpTransceiver;
}

class HttpExportForm : public AbstractParameterEditor
{
    Q_OBJECT

public:
    explicit HttpExportForm(QSharedPointer<ParameterDelegate> delegate);
    ~HttpExportForm() override;

    QString title() override;
    bool setParameters(const Parameters &parameters) override;
    Parameters parameters() override;
    bool isStandaloneDialog() override;

    static QSharedPointer<ExportResult> exportData(QByteArray data, const Parameters &parameters, QSharedPointer<PluginActionProgress> progress);

private slots:
    void on_pb_send_clicked();

private:
    Ui::HttpTransceiver *ui;
    QSharedPointer<ParameterHelper> m_paramHelper;
};

#endif // HTTPEXPORTFORM_H
