#pragma once

#include "operatorinterface.h"
#include "parameterdelegate.h"

class Edit : public QObject, OperatorInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "hobbits.OperatorInterface.Edit")
    Q_INTERFACES(OperatorInterface)

public:
    Edit();

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
    int getUnitSize(Parameters parameters);
    QSharedPointer<BitArray> parseBits(QString newBitsInRange);
    QSharedPointer<BitArray> parseHex(QString newBitsInRange);
    QSharedPointer<BitArray> parseAscii(QString newBitsInRange);

    QSharedPointer<ParameterDelegate> m_delegate;

};

