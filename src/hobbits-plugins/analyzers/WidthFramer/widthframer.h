#ifndef WIDTHFRAMER_H
#define WIDTHFRAMER_H

#include "analyzerinterface.h"
#include "parameterdelegate.h"

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
            const Parameters &parameters,
            QSharedPointer<PluginActionProgress> progress) override;

private:
    QSharedPointer<ParameterDelegate> m_delegate;
};

#endif // WIDTHFRAMER_H
