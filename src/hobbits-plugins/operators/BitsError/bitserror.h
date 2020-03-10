#ifndef BITSERROR_H
#define BITSERROR_H

#include "operatorinterface.h"

namespace Ui
{
class BitsError;
}

class BitsError : public QObject, OperatorInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "hobbits.OperatorInterface.3.BitsError")
    Q_INTERFACES(OperatorInterface)

public:
    BitsError();

    QString getName() override;

    OperatorInterface* createDefaultOperator() override;
    void applyToWidget(QWidget *widget) override;
    void provideCallback(QSharedPointer<PluginCallback> pluginCallback) override;

    bool canRecallPluginState(const QJsonObject &pluginState) override;
    virtual bool setPluginStateInUi(const QJsonObject &pluginState) override;
    QJsonObject getStateFromUi() override;

    int getMinInputContainers(const QJsonObject &pluginState) override;
    int getMaxInputContainers(const QJsonObject &pluginState) override;

    QSharedPointer<const OperatorResult> operateOnContainers(
            QList<QSharedPointer<const BitContainer>> inputContainers,
            const QJsonObject &recallablePluginState,
            QSharedPointer<ActionProgress> progressTracker) override;


    QSharedPointer<OperatorResult> gaussianFlip(
            QList<QSharedPointer<const BitContainer>> inputContainers,
            const QJsonObject &recallablePluginState,
            QSharedPointer<ActionProgress> progressTracker);

private:
    Ui::BitsError *ui;
    QSharedPointer<PluginCallback> m_pluginCallback;

};

#endif // BITSERROR_H
