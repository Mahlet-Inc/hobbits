#ifndef %{JS: '%{ClassName}'.toUpperCase()}_H
#define %{JS: '%{ClassName}'.toUpperCase()}_H


#include "analyzerinterface.h"
#include "parameterdelegateui.h"

class %{ClassName} : public QObject, AnalyzerInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "hobbits.AnalyzerInterface.%{ClassName}")
    Q_INTERFACES(AnalyzerInterface)

public:
    %{ClassName}();

    AnalyzerInterface* createDefaultAnalyzer() override;

    QString name() override;
    QString description() override;
    QStringList tags() override;

    QSharedPointer<ParameterDelegate> parameterDelegate() override;

    QSharedPointer<const AnalyzerResult> analyzeBits(
            QSharedPointer<const BitContainer> container,
            const QJsonObject &parameters,
            QSharedPointer<PluginActionProgress> progress) override;

private:
    QSharedPointer<ParameterDelegateUi> m_delegate;
};

#endif // %{JS: '%{ClassName}'.toUpperCase()}_H
