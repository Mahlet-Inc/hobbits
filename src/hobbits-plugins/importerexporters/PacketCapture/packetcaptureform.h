#ifndef PACKETCAPTUREFORM_H
#define PACKETCAPTUREFORM_H

#include "abstractparametereditor.h"
#include "parameterdelegate.h"
#include "parameterhelper.h"
#include <pcap/pcap.h>

namespace Ui
{
class PacketCaptureForm;
}

class PacketCaptureForm: public AbstractParameterEditor
{
    Q_OBJECT

public:
    explicit PacketCaptureForm(QSharedPointer<ParameterDelegate> delegate);
    ~PacketCaptureForm() override;

    QString title() override;
    bool setParameters(const Parameters &parameters) override;
    Parameters parameters() override;
    bool isStandaloneDialog() override;

private slots:
    void on_pb_beginCapture_clicked();
    void populateDeviceBox();

private:
    Ui::PacketCaptureForm *ui;
    QSharedPointer<ParameterHelper> m_paramHelper;
};

#endif // PACKETCAPTUREFORM_H
