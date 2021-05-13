#ifndef UDPRECEIVER_H
#define UDPRECEIVER_H

#include <QUdpSocket>
#include <QTemporaryFile>
#include "abstractparametereditor.h"
#include "parameterdelegate.h"
#include "parameterhelper.h"
#include "importresult.h"

namespace Ui {
class UdpReceiver;
}

class UdpReceiver : public AbstractParameterEditor
{
    Q_OBJECT

public:
    explicit UdpReceiver(QSharedPointer<ParameterDelegate> delegate);
    ~UdpReceiver() override;

    QString title() override;
    bool setParameters(const Parameters &parameters) override;
    Parameters parameters() override;
    bool isStandaloneDialog() override;

    static QSharedPointer<ImportResult> importData(const Parameters &parameters, QSharedPointer<PluginActionProgress> progress);

private slots:
    void on_pb_start_pressed();

private:
    Ui::UdpReceiver *ui;
    QSharedPointer<ParameterHelper> m_paramHelper;
};

#endif // UDPRECEIVER_H
