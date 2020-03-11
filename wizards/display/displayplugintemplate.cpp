#include "%{HeaderFileName}"
#include "%{FormHeaderFileName}"

%{ClassName}::%{ClassName}() :
    m_renderConfig(new DisplayRenderConfig())
{
    // TODO: set up actual render config for this display (e.g. when it updates, if it's asynchronous, etc)
    m_renderConfig->setFullRedrawTriggers(DisplayRenderConfig::NewBitOffset | DisplayRenderConfig::NewFrameOffset);
    m_renderConfig->setOverlayRedrawTriggers(DisplayRenderConfig::NewBitHover);

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

DisplayInterface* %{ClassName}::createDefaultDisplay()
{
    return new %{ClassName}();
}

QString %{ClassName}::name()
{
    return "%{ClassName}";
}

QString %{ClassName}::description()
{
    // TODO: add description
    return "%{ClassName}";
}

QStringList %{ClassName}::tags()
{
    // TODO: make tags relevant
    return {"Generic"};
}

QSharedPointer<DisplayRenderConfig> %{ClassName}::renderConfig()
{
    return m_renderConfig;
}

void %{ClassName}::setDisplayHandle(QSharedPointer<DisplayHandle> displayHandle)
{
    m_handle = displayHandle;
}

QSharedPointer<ParameterDelegate> %{ClassName}::parameterDelegate()
{
    return m_delegate;
}

QSharedPointer<DisplayResult> %{ClassName}::renderDisplay(QSize viewportSize, const QJsonObject &parameters, QSharedPointer<PluginActionProgress> progress)
{
    if (!m_delegate->validate(parameters)) {
        return DisplayResult::error("Invalid parameters");
    }
    if (m_handle.isNull() || m_handle->currentContainer().isNull()) {
        return DisplayResult::nullResult();
    }

    // TODO: render and return the display image
    return DisplayResult::nullResult();
}

QSharedPointer<DisplayResult> %{ClassName}::renderOverlay(QSize viewportSize, const QJsonObject &parameters)
{
    if (!m_delegate->validate(parameters)) {
        m_handle->setRenderedRange(this, Range());
        return DisplayResult::error("Invalid parameters");
    }
    if (m_handle.isNull() || m_handle->currentContainer().isNull()) {
        m_handle->setRenderedRange(this, Range());
        return DisplayResult::nullResult();
    }

    // TODO: render and return the overlay image
    return DisplayResult::nullResult();
}