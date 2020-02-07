#ifndef %{JS: '%{ClassName}'.toUpperCase()}_H
#define %{JS: '%{ClassName}'.toUpperCase()}_H

#include "analyzerinterface.h"

namespace Ui
{
class %{ClassName};
}

class %{ClassName} : public QObject, AnalyzerInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "hobbits.AnalyzerInterface.%{ClassName}")
    Q_INTERFACES(AnalyzerInterface)

public:
    %{ClassName}();
    ~%{ClassName}() override;

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

private:
    Ui::%{ClassName} *ui;
    QSharedPointer<PluginCallback> m_pluginCallback;
};

#endif // %{JS: '%{ClassName}'.toUpperCase()}_H
