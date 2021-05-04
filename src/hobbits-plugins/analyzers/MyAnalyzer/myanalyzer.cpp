#include "myanalyzer.h"
#include "myanalyzereditor.h"

MyAnalyzer::MyAnalyzer()
{
    QList<ParameterDelegate::ParameterInfo> infos = {
        // TODO: add parameters like {"myparametername", ParameterDelegate::ParameterType::Decimal}
    };

    m_delegate = ParameterDelegate::create(
                    infos,
                    [this](const QJsonObject &parameters) {
                        // TODO: use parameters to describe action better
                        return QString("Apply %1").arg(this->name());
                    },
                    [](QSharedPointer<ParameterDelegate> delegate, QSize size) {
                        Q_UNUSED(size)
                        return new MyAnalyzerEditor(delegate);
                    });
}

AnalyzerInterface* MyAnalyzer::createDefaultAnalyzer()
{
    return new MyAnalyzer();
}

QString MyAnalyzer::name()
{
    return "MyAnalyzer";
}

QString MyAnalyzer::description()
{
    // TODO: create actual description
    return "Describes what MyAnalyzer does";
}

QStringList MyAnalyzer::tags()
{
    // TODO: add relevant tags
    return {"Generic"};
}

QSharedPointer<ParameterDelegate>  MyAnalyzer::parameterDelegate()
{
    return m_delegate;
}

QSharedPointer<const AnalyzerResult> MyAnalyzer::analyzeBits(
        QSharedPointer<const BitContainer> container,
        const QJsonObject &parameters,
        QSharedPointer<PluginActionProgress> progress)
{
    QStringList invalidations = m_delegate->validate(parameters);
    if (!invalidations.isEmpty()) {
        return AnalyzerResult::error(QString("Invalid parameters passed to %1:\n%2").arg(name()).arg(invalidations.join("\n")));
    }

    // TODO: Perform analysis and return result with AnalyzerResult::result
    return AnalyzerResult::error("Plugin analyze action not implemented");
}
