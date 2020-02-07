#ifndef FINDANALYZER_H
#define FINDANALYZER_H

#include "analyzerinterface.h"

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

    QString getName() override;
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
            QSharedPointer<ActionProgress> progressTracker) override;

public slots:
    void resolveFindFocusHighlight();
    void checkFindButtons();
    void initializeFindView();
    void focusOnPrevious();
    void focusOnNext();
    void focusOnIndex(int index);

    void triggerRun();

private:
    BitArray getFindBits(const QJsonObject &pluginState);

    Ui::FindAnalyzer *ui;

    int m_focusIndex;
    QSharedPointer<BitContainerPreview> m_previewContainer;
    QSharedPointer<PluginCallback> m_pluginCallback;
};

#endif // FINDANALYZER_H
