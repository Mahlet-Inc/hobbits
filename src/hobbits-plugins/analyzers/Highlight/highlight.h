#ifndef HIGHLIGHT_H
#define HIGHLIGHT_H


#include "analyzerinterface.h"
#include "parameterdelegateui.h"

class Highlight : public QObject, AnalyzerInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "hobbits.AnalyzerInterface.Highlight")
    Q_INTERFACES(AnalyzerInterface)

public:
    Highlight();

    AnalyzerInterface* createDefaultAnalyzer() override;

    QString name() override;
    QString description() override;
    QStringList tags() override;

    QSharedPointer<ParameterDelegate> parameterDelegate() override;

    QSharedPointer<const AnalyzerResult> analyzeBits(
            QSharedPointer<const BitContainer> container,
            const QJsonObject &parameters,
            QSharedPointer<PluginActionProgress> progress) override;

private:
    QSharedPointer<ParameterDelegateUi> m_delegate;
};

#endif // HIGHLIGHT_H
