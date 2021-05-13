#ifndef TCPSENDER_H
#define TCPSENDER_H

#include <QTcpSocket>
#include "abstractparametereditor.h"
#include "parameterdelegate.h"
#include "parameterhelper.h"
#include "exportresult.h"

namespace Ui {
class TcpSender;
}

class TcpSender : public AbstractParameterEditor
{
    Q_OBJECT

public:
    explicit TcpSender(QSharedPointer<ParameterDelegate> delegate);
    ~TcpSender() override;

    QString title() override;
    bool setParameters(const Parameters &parameters) override;
    Parameters parameters() override;
    bool isStandaloneDialog() override;

    static QSharedPointer<ExportResult> exportData(QSharedPointer<const BitArray> bits, const Parameters &parameters, QSharedPointer<PluginActionProgress> progress);

private slots:
    void on_pb_send_pressed();

private:
    Ui::TcpSender *ui;
    QSharedPointer<ParameterHelper> m_paramHelper;
};

#endif // TCPSENDER_H
