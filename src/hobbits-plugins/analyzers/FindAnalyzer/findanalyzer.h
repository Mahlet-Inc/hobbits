#ifndef FINDANALYZER_H
#define FINDANALYZER_H

#include "analyzerinterface.h"
#include "highlightnavigator.h"

namespace Ui
{
class FindAnalyzer;
}

class FindAnalyzer : public QObject, AnalyzerInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "hobbits.AnalyzerInterface.FindAnalyzer")
    Q_INTERFACES(AnalyzerInterface)

public:
    FindAnalyzer();

    ~FindAnalyzer() override;

    QString name() override;
    void previewBits(QSharedPointer<BitContainerPreview> container) override;
    void provideCallback(QSharedPointer<PluginCallback> pluginCallback) override;

    AnalyzerInterface* createDefaultAnalyzer() override;
    void applyToWidget(QWidget *widget) override;

    bool canRecallPluginState(const QJsonObject &pluginState) override;
    virtual bool setPluginStateInUi(const QJsonObject &pluginState) override;
    QJsonObject getStateFromUi() override;
    QSharedPointer<const AnalyzerResult> analyzeBits(
            QSharedPointer<const BitContainer> container,
            const QJsonObject &recallablePluginState,
            QSharedPointer<PluginActionProgress> progressTracker) override;

public slots:
    void triggerRun();

private:
    QSharedPointer<BitArray> getFindBits(const QJsonObject &pluginState);

    Ui::FindAnalyzer *ui;

    HighlightNavigator* m_highlightNav;
    QSharedPointer<BitContainerPreview> m_previewContainer;
    QSharedPointer<PluginCallback> m_pluginCallback;
};

#endif // FINDANALYZER_H
