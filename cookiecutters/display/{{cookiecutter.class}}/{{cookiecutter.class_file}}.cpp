#include "{{cookiecutter.class|lower}}.h"
#include "{{cookiecutter.editor_class|lower}}.h"
#include "displayresult.h"

{{cookiecutter.class}}::{{cookiecutter.class}}() :
    m_renderConfig(new DisplayRenderConfig())
{
    // TODO: set up actual render config for this display (e.g. when it updates, if it's asynchronous, etc)
    m_renderConfig->setFullRedrawTriggers(DisplayRenderConfig::NewBitOffset | DisplayRenderConfig::NewFrameOffset);
    m_renderConfig->setOverlayRedrawTriggers(DisplayRenderConfig::NewBitHover);

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

DisplayInterface* {{cookiecutter.class}}::createDefaultDisplay()
{
    return new {{cookiecutter.class}}();
}

QString {{cookiecutter.class}}::name()
{
    return "{{cookiecutter.class}}";
}

QString {{cookiecutter.class}}::description()
{
    // TODO: add description
    return "{{cookiecutter.class}}";
}

QStringList {{cookiecutter.class}}::tags()
{
    // TODO: make tags relevant
    return {"Generic"};
}

QSharedPointer<DisplayRenderConfig> {{cookiecutter.class}}::renderConfig()
{
    return m_renderConfig;
}

void {{cookiecutter.class}}::setDisplayHandle(QSharedPointer<DisplayHandle> displayHandle)
{
    m_handle = displayHandle;
}

QSharedPointer<ParameterDelegate> {{cookiecutter.class}}::parameterDelegate()
{
    return m_delegate;
}

QSharedPointer<DisplayResult> {{cookiecutter.class}}::renderDisplay(QSize viewportSize, const QJsonObject &parameters, QSharedPointer<PluginActionProgress> progress)
{
    QStringList invalidations = m_delegate->validate(parameters);
    if (!invalidations.isEmpty()) {
        return DisplayResult::error(QString("Invalid parameters passed to %1:\n%2").arg(name()).arg(invalidations.join("\n")));
    }
    if (m_handle.isNull() || m_handle->currentContainer().isNull()) {
        return DisplayResult::nullResult();
    }

    // TODO: render and return the display image
    return DisplayResult::nullResult();
}

QSharedPointer<DisplayResult> {{cookiecutter.class}}::renderOverlay(QSize viewportSize, const QJsonObject &parameters)
{
    QStringList invalidations = m_delegate->validate(parameters);
    if (!invalidations.isEmpty()) {
        m_handle->setRenderedRange(this, Range());
        return DisplayResult::error(QString("Invalid parameters passed to %1:\n%2").arg(name()).arg(invalidations.join("\n")));
    }
    if (m_handle.isNull() || m_handle->currentContainer().isNull()) {
        m_handle->setRenderedRange(this, Range());
        return DisplayResult::nullResult();
    }

    // TODO: render and return the overlay image
    return DisplayResult::nullResult();
}
