#ifndef PYTHONRUNNER_H
#define PYTHONRUNNER_H

#include "mathparser.h"
#include "operatorinterface.h"
#include "pluginstatehelper.h"


namespace Ui
{
class PythonRunner;

}

class PythonRunner : public QObject, OperatorInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "hobbits.OperatorInterface.3.PythonRunner")
    Q_INTERFACES(OperatorInterface)

public:
    PythonRunner();

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

public Q_SLOTS:
    void openPythonPathDialog();
    void openHelpDialog();

    void updateOutputText(QString);
    void clearOutputText();

private:
    Ui::PythonRunner *ui;
    QSharedPointer<PluginCallback> m_pluginCallback;
    QSharedPointer<PluginStateHelper> m_stateHelper;
    QString m_outputText;
};

#endif // PYTHONRUNNER_H
