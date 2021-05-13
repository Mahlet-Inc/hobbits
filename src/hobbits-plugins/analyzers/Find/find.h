#ifndef FIND_H
#define FIND_H


#include "analyzerinterface.h"
#include "parameterdelegate.h"

class Find : public QObject, AnalyzerInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "hobbits.AnalyzerInterface.Find")
    Q_INTERFACES(AnalyzerInterface)

public:
    Find();

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

#endif // FIND_H
