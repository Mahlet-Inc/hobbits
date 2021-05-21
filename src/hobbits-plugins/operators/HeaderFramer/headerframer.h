#ifndef HEADERFRAMER_H
#define HEADERFRAMER_H

#include "parameterdelegate.h"
#include "operatorinterface.h"

class HeaderFramer : public QObject, OperatorInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "hobbits.OperatorInterface.3.HeaderFramer")
    Q_INTERFACES(OperatorInterface)

public:
    struct HeaderInfo {
        QSharedPointer<BitArray> headerBits;
        int frameLength;
        int prePadLength;
        bool byteAligned;
    };

    HeaderFramer();

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

#endif // HEADERFRAMER_H
