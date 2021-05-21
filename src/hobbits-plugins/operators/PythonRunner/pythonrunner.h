#ifndef PYTHONRUNNER_H
#define PYTHONRUNNER_H

#include "mathparser.h"
#include "operatorinterface.h"
#include "parameterdelegate.h"

class PythonRunner : public QObject, OperatorInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "hobbits.OperatorInterface.PythonRunner")
    Q_INTERFACES(OperatorInterface)

public:
    PythonRunner();

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

#endif // PYTHONRUNNER_H
