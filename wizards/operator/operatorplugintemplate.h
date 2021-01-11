#ifndef %{JS: '%{ClassName}'.toUpperCase()}_H
#define %{JS: '%{ClassName}'.toUpperCase()}_H

#include "operatorinterface.h"
#include "pluginstatehelper.h"

namespace Ui
{
class %{ClassName};

}

class %{ClassName} : public QObject, OperatorInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "hobbits.OperatorInterface.4.%{ClassName}")
    Q_INTERFACES(OperatorInterface)

public:
    %{ClassName}();

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
    QSharedPointer<ParameterDelegate> m_delegate;
};

#endif // %{JS: '%{ClassName}'.toUpperCase()}_H
