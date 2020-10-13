#include "%{HeaderFileName}"
#include "%{FormHeaderFileName}"

%{ClassName}::%{ClassName}()
{
    QList<ParameterDelegate::ParameterInfo> infos = {
        // TODO: add parameters like {"myparametername", QJsonValue::Double}
    };

    m_delegate = QSharedPointer<ParameterDelegateUi>(
                new ParameterDelegateUi(
                    infos,
                    [this](const QJsonObject &parameters) {
                        // TODO: use parameters to describe action better
                        return QString("Apply %1").arg(this->name());
                    },
                    [](QSharedPointer<ParameterDelegate> delegate, QSize size) {
                        Q_UNUSED(size)
                        return new %{FormClassName}(delegate);
                    }));
}

AnalyzerInterface* %{ClassName}::createDefaultAnalyzer()
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

QSharedPointer<const AnalyzerResult> %{ClassName}::analyzeBits(
        QSharedPointer<const BitContainer> container,
        const QJsonObject &parameters,
        QSharedPointer<PluginActionProgress> progress)
{
    if (!m_delegate->validate(parameters)) {
        return AnalyzerResult::error(QString("Invalid parameters passed to %1").arg(name()));
    }

    // TODO: Perform analysis and return result with AnalyzerResult::result
    return AnalyzerResult::error("Plugin analyze action not implemented");
}
