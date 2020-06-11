#ifndef BITSERROR_H
#define BITSERROR_H

#include "operatorinterface.h"
#include "pluginstatehelper.h"

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
    void previewBits(QSharedPointer<BitContainerPreview> container) override;

private:
    QSharedPointer<const OperatorResult> getGaussianErrorBits(QSharedPointer<const BitContainer> input,
                                                      const QJsonObject &recallablePluginState,
                                                      QSharedPointer<ActionProgress> progressTracker);
    QSharedPointer<const OperatorResult> getPeriodicErrorBits(QSharedPointer<const BitContainer> input,
                                                        const QJsonObject &recallablePluginState,
                                                        QSharedPointer<ActionProgress> progressTracker);
    Ui::BitsError *ui;
    QSharedPointer<PluginCallback> m_pluginCallback;
    QSharedPointer<PluginStateHelper> m_stateHelper;

};

#endif // BITSERROR_H
