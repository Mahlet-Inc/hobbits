#ifndef EXTRACTOR_H
#define EXTRACTOR_H

#include "operatorinterface.h"
#include "highlightnavigator.h"
#include "parameterdelegate.h"

namespace Ui
{
class Extractor;

}

class Extractor : public QObject, OperatorInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "hobbits.OperatorInterface.Extractor")
    Q_INTERFACES(OperatorInterface)

public:
    Extractor();

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

#endif // EXTRACTOR_H
