#ifndef TAKESKIPOPERATOR_H
#define TAKESKIPOPERATOR_H

#include "operatorinterface.h"
#include "parameterdelegate.h"

class TakeSkip : public QObject, OperatorInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "hobbits.OperatorInterface.TakeSkip")
    Q_INTERFACES(OperatorInterface)

public:
    TakeSkip();

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

#endif // TAKESKIPOPERATOR_H
