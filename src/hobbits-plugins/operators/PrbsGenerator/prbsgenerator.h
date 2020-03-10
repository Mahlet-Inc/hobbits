#ifndef PRBSGENERATOR_H
#define PRBSGENERATOR_H

#include "mathparser.h"
#include "operatorinterface.h"


namespace Ui
{
class PrbsGenerator;

}

class PrbsGenerator : public QObject, OperatorInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "hobbits.OperatorInterface.3.PrbsGenerator")
    Q_INTERFACES(OperatorInterface)

public:
    PrbsGenerator();

    QString getName() override;

    OperatorInterface* createDefaultOperator() override;
    void applyToWidget(QWidget *widget) override;
    void provideCallback(QSharedPointer<PluginCallback> pluginCallback) override;
    void updateEndState(QString lfsr);

    bool canRecallPluginState(const QJsonObject &pluginState) override;
    virtual bool setPluginStateInUi(const QJsonObject &pluginState) override;
    QJsonObject getStateFromUi() override;
    int getMinInputContainers(const QJsonObject &pluginState) override;
    int getMaxInputContainers(const QJsonObject &pluginState) override;

    QSharedPointer<const OperatorResult> operateOnContainers(
            QList<QSharedPointer<const BitContainer>> inputContainers,
            const QJsonObject &recallablePluginState,
            QSharedPointer<ActionProgress> progressTracker) override;

private slots:
    void showHelp();

private:
    Ui::PrbsGenerator *ui;
    QSharedPointer<PluginCallback> m_pluginCallback;
};

#endif // PRBSGENERATOR_H
