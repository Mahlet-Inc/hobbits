#ifndef UDPSENDER_H
#define UDPSENDER_H

#include <QUdpSocket>
#include "bitarray.h"
#include "abstractparametereditor.h"
#include "parameterdelegate.h"
#include "parameterhelper.h"
#include "exportresult.h"

namespace Ui {
class UdpSender;
}

class UdpSender : public AbstractParameterEditor
{
    Q_OBJECT

public:
    explicit UdpSender(QSharedPointer<ParameterDelegate> delegate);
    ~UdpSender() override;

    QString title() override;
    bool setParameters(QJsonObject parameters) override;
    QJsonObject parameters() override;
    bool isStandaloneDialog() override;

    static QSharedPointer<ExportResult> exportData(QSharedPointer<const BitArray> bits, QJsonObject parameters, QSharedPointer<PluginActionProgress> progress);

private slots:
    void on_pb_send_pressed();

private:
    Ui::UdpSender *ui;
    QSharedPointer<ParameterHelper> m_paramHelper;

};

#endif // UDPSENDER_H
