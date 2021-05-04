#include "{{cookiecutter.class|lower}}.h"
#include "{{cookiecutter.editor_class|lower}}.h"

{{cookiecutter.class}}::{{cookiecutter.class}}()
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
                        return new {{cookiecutter.editor_class}}(delegate);
                    });
}

OperatorInterface* {{cookiecutter.class}}::createDefaultOperator()
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

int {{cookiecutter.class}}::getMinInputContainers(const QJsonObject &pluginState)
{
    Q_UNUSED(pluginState)
    return 1;
}

int {{cookiecutter.class}}::getMaxInputContainers(const QJsonObject &pluginState)
{
    Q_UNUSED(pluginState)
    return 1;
}

QSharedPointer<const OperatorResult> {{cookiecutter.class}}::operateOnBits(
    QList<QSharedPointer<const BitContainer> > inputContainers,
    const QJsonObject &parameters,
    QSharedPointer<PluginActionProgress> progress)
{     
    QStringList invalidations = m_delegate->validate(parameters);
    if (!invalidations.isEmpty()) {
        return OperatorResult::error(QString("Invalid parameters passed to %1:\n%2").arg(name()).arg(invalidations.join("\n")));
    }

    // TODO: Perform analysis and return result with OperatorResult::result
    return OperatorResult::error("Plugin operate action not implemented");
}
