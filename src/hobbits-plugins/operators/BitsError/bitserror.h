#ifndef BITSERROR_H
#define BITSERROR_H

#include "operatorinterface.h"
#include "parameterdelegate.h"

class BitsError : public QObject, OperatorInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "hobbits.OperatorInterface.BitsError")
    Q_INTERFACES(OperatorInterface)

public:
    BitsError();

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
    QSharedPointer<const OperatorResult> getGaussianErrorBits(QSharedPointer<const BitContainer> input,
                                                      double ber,
                                                      const Parameters &parameters,
                                                      QSharedPointer<PluginActionProgress> progressTracker);
    QSharedPointer<const OperatorResult> getPeriodicErrorBits(QSharedPointer<const BitContainer> input,
                                                      double ber,
                                                      const Parameters &parameters,
                                                      QSharedPointer<PluginActionProgress> progressTracker);
    QSharedPointer<ParameterDelegate> m_delegate;

};

#endif // BITSERROR_H
