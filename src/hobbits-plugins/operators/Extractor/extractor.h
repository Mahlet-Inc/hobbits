#ifndef EXTRACTOR_H
#define EXTRACTOR_H

#include "operatorinterface.h"
#include "highlightnavigator.h"
#include "parameterdelegateui.h"

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

    int getMinInputContainers(const QJsonObject &pluginState) override;
    int getMaxInputContainers(const QJsonObject &pluginState) override;

    QSharedPointer<const OperatorResult> operateOnBits(
            QList<QSharedPointer<const BitContainer>> inputContainers,
            const QJsonObject &parameters,
            QSharedPointer<PluginActionProgress> progress) override;


private:
    QSharedPointer<ParameterDelegateUi> m_delegate;
};

#endif // EXTRACTOR_H
