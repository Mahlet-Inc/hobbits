#ifndef TAKESKIPOPERATOR_H
#define TAKESKIPOPERATOR_H

#include "operatorinterface.h"
#include "parameterdelegateui.h"

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

    int getMinInputContainers(const QJsonObject &pluginState) override;
    int getMaxInputContainers(const QJsonObject &pluginState) override;

    QSharedPointer<const OperatorResult> operateOnBits(
            QList<QSharedPointer<const BitContainer>> inputContainers,
            const QJsonObject &parameters,
            QSharedPointer<PluginActionProgress> progress) override;

private:
    QSharedPointer<ParameterDelegateUi> m_delegate;


};

#endif // TAKESKIPOPERATOR_H
