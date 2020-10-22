#ifndef PYTHONANALYZER_H
#define PYTHONANALYZER_H

#include "analyzerinterface.h"
#include "parameterdelegate.h"
#include "pythonpluginconfig.h"
#include "hobbits-python_global.h"

class HOBBITSPYTHONSHARED_EXPORT PythonAnalyzer : public AnalyzerInterface
{
public:
    PythonAnalyzer(QSharedPointer<PythonPluginConfig> config);

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
    QSharedPointer<PythonPluginConfig> m_config;
};

#endif // PYTHONANALYZER_H
