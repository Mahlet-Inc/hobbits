#ifndef HEADERFRAMER_H
#define HEADERFRAMER_H

#include "parameterdelegateui.h"
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

    int getMinInputContainers(const QJsonObject &pluginState) override;
    int getMaxInputContainers(const QJsonObject &pluginState) override;

    QSharedPointer<const OperatorResult> operateOnBits(
            QList<QSharedPointer<const BitContainer>> inputContainers,
            const QJsonObject &parameters,
            QSharedPointer<PluginActionProgress> progress) override;

private:

    QSharedPointer<ParameterDelegateUi> m_delegate;

};

#endif // HEADERFRAMER_H
