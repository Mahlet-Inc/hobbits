#ifndef KAITAISTRUCT_H
#define KAITAISTRUCT_H

#include "operatorinterface.h"

namespace Ui
{
class KaitaiStruct;

}

class KaitaiStruct : public QObject, OperatorInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "hobbits.OperatorInterface.4.KaitaiStruct")
    Q_INTERFACES(OperatorInterface)

public:
    KaitaiStruct();

    OperatorInterface* createDefaultOperator() override;
    QString getName() override;

    void provideCallback(QSharedPointer<PluginCallback> pluginCallback) override;
    void applyToWidget(QWidget *widget) override;

    bool canRecallPluginState(const QJsonObject& pluginState) override;
    bool setPluginStateInUi(const QJsonObject &pluginState) override;
    QJsonObject getStateFromUi() override;

    int getMinInputContainers(const QJsonObject &pluginState) override;
    int getMaxInputContainers(const QJsonObject &pluginState) override;

    QSharedPointer<const OperatorResult> operateOnContainers(
            QList<QSharedPointer<const BitContainer> > inputContainers,
            const QJsonObject &recallablePluginState,
            QSharedPointer<ActionProgress> progressTracker) override;

private slots:
    void openPythonPathDialog();
    void openKscPathDialog();
    void updateOutputText(QString text);
    void clearOutputText();

private:
    Ui::KaitaiStruct *ui;
    QSharedPointer<PluginCallback> m_pluginCallback;
};

#endif // KAITAISTRUCT_H
