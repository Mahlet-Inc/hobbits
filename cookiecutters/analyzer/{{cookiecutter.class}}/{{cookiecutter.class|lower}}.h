#ifndef {{cookiecutter.class|upper}}
#define {{cookiecutter.class|upper}}


#include "analyzerinterface.h"
#include "parameterdelegate.h"

class {{cookiecutter.class}} : public QObject, AnalyzerInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "hobbits.AnalyzerInterface.{{cookiecutter.class}}")
    Q_INTERFACES(AnalyzerInterface)

public:
    {{cookiecutter.class}}();

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
    QSharedPointer<ParameterDelegate> m_delegate;
};

#endif // {{cookiecutter.class|upper}}
