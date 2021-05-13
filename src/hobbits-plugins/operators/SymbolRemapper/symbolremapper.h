#ifndef SYMBOLREMAPPER_H
#define SYMBOLREMAPPER_H

#include "operatorinterface.h"

class SymbolRemapper : public QObject, OperatorInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "hobbits.OperatorInterface.SymbolRemapper")
    Q_INTERFACES(OperatorInterface)

public:
    SymbolRemapper();

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

#endif // SYMBOLREMAPPER_H
