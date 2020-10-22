#include "%{HeaderFileName}"
#include "%{FormHeaderFileName}"

%{ClassName}::%{ClassName}()
{
    QList<ParameterDelegate::ParameterInfo> infos = {
        // TODO: add parameters like {"myparametername", QJsonValue::Double}
    };

    m_delegate = ParameterDelegate::create(
                    infos,
                    [this](const QJsonObject &parameters) {
                        // TODO: use parameters to describe action better
                        return QString("Apply %1").arg(this->name());
                    },
                    [](QSharedPointer<ParameterDelegate> delegate, QSize size) {
                        Q_UNUSED(size)
                        return new %{FormClassName}(delegate);
                    });
}

OperatorInterface* %{ClassName}::createDefaultOperator()
{
    return new %{ClassName}();
}

QString %{ClassName}::name()
{
    return "%{ClassName}";
}

QString %{ClassName}::description()
{
    // TODO: create actual description
    return "Describes what %{ClassName} does";
}

QStringList %{ClassName}::tags()
{
    // TODO: add relevant tags
    return {"Generic"};
}

QSharedPointer<ParameterDelegate>  %{ClassName}::parameterDelegate()
{
    return m_delegate;
}

int %{ClassName}::getMinInputContainers(const QJsonObject &pluginState)
{
    Q_UNUSED(pluginState)
    return 1;
}

int %{ClassName}::getMaxInputContainers(const QJsonObject &pluginState)
{
    Q_UNUSED(pluginState)
    return 1;
}

QSharedPointer<const OperatorResult> %{ClassName}::operateOnBits(
    QList<QSharedPointer<const BitContainer> > inputContainers,
    const QJsonObject &parameters,
    QSharedPointer<ActionProgress> progress)
{     
    if (!m_delegate->validate(parameters)) {
        return OperatorResult::error(QString("Invalid parameters passed to %1").arg(name()));
    }

    // TODO: Perform analysis and return result with OperatorResult::result
    return OperatorResult::error("Plugin operate action not implemented");
}
