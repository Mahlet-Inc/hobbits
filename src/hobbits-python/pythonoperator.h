#ifndef PYTHONOPERATOR_H
#define PYTHONOPERATOR_H

#include "operatorinterface.h"
#include "parameterdelegate.h"
#include "pythonpluginconfig.h"
#include "hobbits-python_global.h"

class HOBBITSPYTHONSHARED_EXPORT PythonOperator : public OperatorInterface
{
public:
    PythonOperator(QSharedPointer<PythonPluginConfig> config);

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
    QSharedPointer<PythonPluginConfig> m_config;
};

#endif // PYTHONOPERATOR_H
