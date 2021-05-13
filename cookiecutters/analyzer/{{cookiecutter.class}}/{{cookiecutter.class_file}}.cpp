#include "{{cookiecutter.class|lower}}.h"
#include "{{cookiecutter.editor_class|lower}}.h"

{{cookiecutter.class}}::{{cookiecutter.class}}()
{
    QList<ParameterDelegate::ParameterInfo> infos = {
        // TODO: add parameters like {"myparametername", ParameterDelegate::ParameterType::Decimal}
    };

    m_delegate = ParameterDelegate::create(
                    infos,
                    [this](const Parameters &parameters) {
                        // TODO: use parameters to describe action better
                        return QString("Apply %1").arg(this->name());
                    },
                    [](QSharedPointer<ParameterDelegate> delegate, QSize size) {
                        Q_UNUSED(size)
                        return new {{cookiecutter.editor_class}}(delegate);
                    });
}

AnalyzerInterface* {{cookiecutter.class}}::createDefaultAnalyzer()
{
    return new {{cookiecutter.class}}();
}

QString {{cookiecutter.class}}::name()
{
    return "{{cookiecutter.class}}";
}

QString {{cookiecutter.class}}::description()
{
    // TODO: create actual description
    return "Describes what {{cookiecutter.class}} does";
}

QStringList {{cookiecutter.class}}::tags()
{
    // TODO: add relevant tags
    return {"Generic"};
}

QSharedPointer<ParameterDelegate>  {{cookiecutter.class}}::parameterDelegate()
{
    return m_delegate;
}

QSharedPointer<const AnalyzerResult> {{cookiecutter.class}}::analyzeBits(
        QSharedPointer<const BitContainer> container,
        const Parameters &parameters,
        QSharedPointer<PluginActionProgress> progress)
{
    QStringList invalidations = m_delegate->validate(parameters);
    if (!invalidations.isEmpty()) {
        return AnalyzerResult::error(QString("Invalid parameters passed to %1:\n%2").arg(name()).arg(invalidations.join("\n")));
    }

    // TODO: Perform analysis and return result with AnalyzerResult::result
    return AnalyzerResult::error("Plugin analyze action not implemented");
}
