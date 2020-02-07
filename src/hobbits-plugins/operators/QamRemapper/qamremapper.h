#ifndef QAMREMAPPER_H
#define QAMREMAPPER_H

#include "mathparser.h"
#include "operatorinterface.h"
#include "remapmodel.h"

namespace Ui
{
class QamRemapper;

}

class QamRemapper : public QObject, OperatorInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "hobbits.OperatorInterface.3.QamRemapper")
    Q_INTERFACES(OperatorInterface)

public:
    QamRemapper();

    ~QamRemapper() override;

    QString getName() override;

    OperatorInterface* createDefaultOperator() override;
    void applyToWidget(QWidget *widget) override;
    void provideCallback(QSharedPointer<PluginCallback> pluginCallback) override;

    bool canRecallPluginState(const QJsonObject &pluginState) override;
    virtual bool setPluginStateInUi(const QJsonObject &pluginState) override;
    QJsonObject getStateFromUi() override;
    int getMinInputContainers() override;
    int getMaxInputContainers() override;

    QSharedPointer<const OperatorResult> operateOnContainers(
            QList<QSharedPointer<const BitContainer>> inputContainers,
            const QJsonObject &recallablePluginState,
            QSharedPointer<ActionProgress> progressTracker) override;

private:
    Ui::QamRemapper *ui;
    RemapModel *m_remapModel;
    QSharedPointer<PluginCallback> m_pluginCallback;

private slots:
    void updateMappings();

};

#endif // QAMREMAPPER_H
