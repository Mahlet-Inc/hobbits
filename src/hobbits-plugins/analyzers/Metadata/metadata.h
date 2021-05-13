#ifndef METADATA_H
#define METADATA_H


#include "analyzerinterface.h"
#include "parameterdelegate.h"

class Metadata : public QObject, AnalyzerInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "hobbits.AnalyzerInterface.Metadata")
    Q_INTERFACES(AnalyzerInterface)

public:
    Metadata();

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

#endif // METADATA_H
