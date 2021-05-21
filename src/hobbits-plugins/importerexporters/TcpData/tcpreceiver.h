#ifndef TCPRECEIVER_H
#define TCPRECEIVER_H

#include <QTcpServer>
#include <QTemporaryFile>
#include "abstractparametereditor.h"
#include "parameterdelegate.h"
#include "parameterhelper.h"
#include "importresult.h"

namespace Ui {
class TcpReceiver;
}

class TcpReceiver : public AbstractParameterEditor
{
    Q_OBJECT

public:
    explicit TcpReceiver(QSharedPointer<ParameterDelegate> delegate);
    ~TcpReceiver() override;

    QString title() override;
    bool setParameters(const Parameters &parameters) override;
    Parameters parameters() override;
    bool isStandaloneDialog() override;

    static QSharedPointer<ImportResult> importData(const Parameters &parameters, QSharedPointer<PluginActionProgress> progress);

private slots:
    void on_pb_start_pressed();

private:
    Ui::TcpReceiver *ui;
    QSharedPointer<ParameterHelper> m_paramHelper;
};

#endif // TCPRECEIVER_H
