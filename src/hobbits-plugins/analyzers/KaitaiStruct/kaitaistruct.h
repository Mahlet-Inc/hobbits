#ifndef KAITAISTRUCT_H
#define KAITAISTRUCT_H

#include "analyzerinterface.h"
#include "highlightnavigator.h"

namespace Ui
{
class KaitaiStruct;
}

class KaitaiStruct : public QObject, AnalyzerInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "hobbits.AnalyzerInterface.KaitaiStruct")
    Q_INTERFACES(AnalyzerInterface)

public:
    KaitaiStruct();
    ~KaitaiStruct() override;

    AnalyzerInterface* createDefaultAnalyzer() override;
    QString getName() override;

    void provideCallback(QSharedPointer<PluginCallback> pluginCallback) override;
    void applyToWidget(QWidget *widget) override;

    bool canRecallPluginState(const QJsonObject& pluginState) override;
    bool setPluginStateInUi(const QJsonObject &pluginState) override;
    QJsonObject getStateFromUi() override;

    void previewBits(QSharedPointer<BitContainerPreview> container) override;
    QSharedPointer<const AnalyzerResult> analyzeBits(
            QSharedPointer<const BitContainer> container,
            const QJsonObject &recallablePluginState,
            QSharedPointer<ActionProgress> progressTracker) override;

private slots:
    void openPythonPathDialog();
    void openKscPathDialog();
    void updateOutputText(QString text);
    void clearOutputText();

private:
    Ui::KaitaiStruct *ui;
    HighlightNavigator* m_highlightNav;
    QSharedPointer<BitContainerPreview> m_previewContainer;
    QSharedPointer<PluginCallback> m_pluginCallback;
};

#endif // KAITAISTRUCT_H
