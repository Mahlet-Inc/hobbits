#ifndef PYTHONOPERATOR_H
#define PYTHONOPERATOR_H

#include "operatorinterface.h"
#include "parameterdelegate.h"
#include "pythonpluginconfig.h"

class PythonOperator : public OperatorInterface
{
public:
    PythonOperator(QSharedPointer<PythonPluginConfig> config);

    OperatorInterface* createDefaultOperator() override;

    QString name() override;
    QString description() override;
    QStringList tags() override;

    QSharedPointer<ParameterDelegate> parameterDelegate() override;

    int getMinInputContainers(const QJsonObject &pluginState) override;
    int getMaxInputContainers(const QJsonObject &pluginState) override;

    QSharedPointer<const OperatorResult> operateOnBits(
            QList<QSharedPointer<const BitContainer>> inputContainers,
            const QJsonObject &parameters,
            QSharedPointer<PluginActionProgress> progress) override;

private:
    QSharedPointer<PythonPluginConfig> m_config;
};

#endif // PYTHONOPERATOR_H
