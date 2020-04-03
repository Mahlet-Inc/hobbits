#ifndef EXTRACTOR_H
#define EXTRACTOR_H

#include "operatorinterface.h"
#include "highlightnavigator.h"

namespace Ui
{
class Extractor;

}

class Extractor : public QObject, OperatorInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "hobbits.OperatorInterface.4.Extractor")
    Q_INTERFACES(OperatorInterface)

public:
    Extractor();

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
    void previewBits(QSharedPointer<BitContainerPreview> container) override;

private slots:
    void setHighlightCategory(QString);
    void sendPluginConfigUpdate();

private:
    Ui::Extractor *ui;
    HighlightNavigator* m_highlightNav;
    QSharedPointer<BitContainerPreview> m_previewContainer;
    QSharedPointer<PluginCallback> m_pluginCallback;
};

#endif // EXTRACTOR_H
