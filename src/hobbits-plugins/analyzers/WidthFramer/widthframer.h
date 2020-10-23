#ifndef WIDTHFRAMER_H
#define WIDTHFRAMER_H

#include "analyzerinterface.h"
#include "parameterdelegateui.h"

class WidthFramer : public QObject, AnalyzerInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "hobbits.AnalyzerInterface.WidthFramer")
    Q_INTERFACES(AnalyzerInterface)

public:
    WidthFramer();

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

#endif // WIDTHFRAMER_H
