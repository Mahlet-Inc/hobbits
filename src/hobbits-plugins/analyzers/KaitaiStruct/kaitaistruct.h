#ifndef KAITAISTRUCT_H
#define KAITAISTRUCT_H

#include "analyzerinterface.h"
#include "highlightnavigator.h"
#include <QMenu>

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
    QString name() override;

    void provideCallback(QSharedPointer<PluginCallback> pluginCallback) override;
    void applyToWidget(QWidget *widget) override;

    bool canRecallPluginState(const QJsonObject& pluginState) override;
    bool setPluginStateInUi(const QJsonObject &pluginState) override;
    QJsonObject getStateFromUi() override;

    void previewBits(QSharedPointer<BitContainerPreview> container) override;
    QSharedPointer<const AnalyzerResult> analyzeBits(
            QSharedPointer<const BitContainer> container,
            const QJsonObject &recallablePluginState,
            QSharedPointer<PluginActionProgress> progressTracker) override;

private slots:
    void openKscPathDialog();
    void updateOutputText(QString text);
    void clearOutputText();

private:
    int m_highlightColorIdx;
    RangeHighlight makeHighlight(QString label, const QMap<QString, QPair<Range, QList<QString>>> &rangeData);

    Ui::KaitaiStruct *ui;
    QMenu* m_loadKsyMenu;
    QMenu* m_loadKsyPyMenu;
    HighlightNavigator* m_highlightNav;
    QSharedPointer<BitContainerPreview> m_previewContainer;
    QSharedPointer<PluginCallback> m_pluginCallback;
    QString m_selectedPrecompiledFile;

};

#endif // KAITAISTRUCT_H
