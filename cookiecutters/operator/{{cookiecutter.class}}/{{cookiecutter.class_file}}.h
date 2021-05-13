#ifndef {{cookiecutter.class|upper}}_H
#define {{cookiecutter.class|upper}}_H

#include "operatorinterface.h"
#include "parameterdelegate.h"

class {{cookiecutter.class}} : public QObject, OperatorInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "hobbits.OperatorInterface.{{cookiecutter.class}}")
    Q_INTERFACES(OperatorInterface)

public:
    {{cookiecutter.class}}();

    OperatorInterface* createDefaultOperator() override;

    QString name() override;
    QString description() override;
    QStringList tags() override;

    QSharedPointer<ParameterDelegate> parameterDelegate() override;

    int getMinInputContainers(const Parameters &parameters) override;
    int getMaxInputContainers(const Parameters &parameters) override;

    QSharedPointer<const OperatorResult> operateOnBits(
            QList<QSharedPointer<const BitContainer>> inputContainers,
            const Parameters &parameters,
            QSharedPointer<PluginActionProgress> progress) override;

private:
    QSharedPointer<ParameterDelegate> m_delegate;
};

#endif // {{cookiecutter.class|upper}}_H
