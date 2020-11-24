#include "byteraster.h"
#include "byterastercontrols.h"
#include "displayhelper.h"
#include <QPainter>
#include <QtMath>

ByteRaster::ByteRaster() :
    m_renderConfig(new DisplayRenderConfig())
{
    m_renderConfig->setFullRedrawTriggers(DisplayRenderConfig::NewBitOffset | DisplayRenderConfig::NewFrameOffset);
    m_renderConfig->setOverlayRedrawTriggers(DisplayRenderConfig::NewBitHover);

    QList<ParameterDelegate::ParameterInfo> infos = {
        {"scale", QJsonValue::Double},
        {"show_headers", QJsonValue::Bool}
    };

    m_delegate = ParameterDelegate::create(
                    infos,
                    [](const QJsonObject &parameters) {
                        int scale = parameters.value("scale").toInt();
                        if (parameters.value("show_headers").toBool()) {
                            return QString("Byte Raster %1x with headers").arg(scale);
                        }
                        else {
                            return QString("Byte Raster %1x without headers").arg(scale);
                        }
                    },
                    [](QSharedPointer<ParameterDelegate> delegate, QSize size) {
                        Q_UNUSED(size)
                        return new ByteRasterControls(delegate);
                    });
}

DisplayInterface* ByteRaster::createDefaultDisplay()
{
    return new ByteRaster();
}

QString ByteRaster::name()
{
    return "Byte Raster";
}

QString ByteRaster::description()
{
    return "Displays each byte in the data as a single colored pixel";
}

QStringList ByteRaster::tags()
{
    return {"Generic"};
}

QSharedPointer<DisplayRenderConfig> ByteRaster::renderConfig()
{
    return m_renderConfig;
}

void ByteRaster::setDisplayHandle(QSharedPointer<DisplayHandle> displayHandle)
{
    m_handle = displayHandle;
    connect(m_handle.data(), &DisplayHandle::newMouseHover, this, [this](DisplayInterface *display, QPoint hover) {
        if (display != this) {
            return;
        }
        if (hover.isNull()
                || m_handle->currentContainer().isNull()
                || !m_delegate->validate(m_lastParams)) {
            m_handle->setBitHover(false);
            return;
        }

        QPoint offset = headerOffset(m_lastParams);
        int scale = m_lastParams.value("scale").toInt();

        qint64 frameOffset = ((hover.y() - offset.y()) / scale);
        qint64 byteOffset = ((hover.x() - offset.x()) / scale);

        m_handle->setBitHover(true, byteOffset * 8, frameOffset);
    });
}

QSharedPointer<ParameterDelegate> ByteRaster::parameterDelegate()
{
    return m_delegate;
}

QImage ByteRaster::renderDisplay(QSize viewportSize, const QJsonObject &parameters, QSharedPointer<PluginActionProgress> progress)
{
    Q_UNUSED(progress)
    m_lastParams = parameters;
    if (m_handle.isNull() || m_handle->currentContainer().isNull() || !m_delegate->validate(m_lastParams)) {
        m_handle->setRenderedRange(this, Range());
        return QImage();
    }

    int scale = parameters.value("scale").toInt();
    QPoint offset = headerOffset(parameters);
    QSize rasterSize(viewportSize.width() - offset.x(), viewportSize.height() - offset.y());
    QSize sourceSize(qMax(1, rasterSize.width() / scale), qMax(1, rasterSize.height() / scale));

    QImage raster = DisplayHelper::getByteRasterImage(
                m_handle->currentContainer(),
                m_handle->bitOffset(),
                m_handle->frameOffset(),
                sourceSize.width(),
                sourceSize.height());

    QImage destImage(viewportSize, QImage::Format_ARGB32);
    destImage.fill(Qt::transparent);
    QPainter painter(&destImage);
    painter.translate(offset);
    painter.scale(scale, scale);
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.drawImage(0, 0, raster);

    painter.resetTransform();
    painter.translate(offset);
    QSize highlightSize(sourceSize.width() * 8, sourceSize.height());
    DisplayHelper::drawHighlights(
                m_handle,
                &painter,
                QSizeF(double(scale) / 8.0, scale),
                highlightSize,
                (m_handle->bitOffset() / 8) * 8,
                m_handle->frameOffset(),
                1);

    DisplayHelper::setRenderRange(this, m_handle, sourceSize.height());

    return destImage;
}

QImage ByteRaster::renderOverlay(QSize viewportSize, const QJsonObject &parameters)
{
    m_lastParams = parameters;
    if (!m_delegate->validate(m_lastParams)) {
        return QImage();
    }

    auto offset = headerOffset(parameters);
    if (offset.x() == 0 && offset.y() == 0) {
        return QImage();
    }

    QImage headers(viewportSize, QImage::Format_ARGB32);
    headers.fill(Qt::transparent);
    QPainter painter(&headers);
    painter.fillRect(0, 0, offset.x(), offset.y(), DisplayHelper::headerBackgroundColor());
    int scale = parameters.value("scale").toInt();

    painter.translate(offset);
    DisplayHelper::drawFramesHeader(&painter,
                                    QSize(offset.x(), viewportSize.height() - offset.y()),
                                    m_handle,
                                    scale);

    DisplayHelper::drawFramesHeader(&painter,
                                    QSize(viewportSize.width() - offset.x(), offset.y()),
                                    m_handle,
                                    double(scale) / 8.0,
                                    Qt::Horizontal);

    return headers;
}

QPoint ByteRaster::headerOffset(const QJsonObject &parameters)
{
    if (!parameters.value("show_headers").toBool() || m_handle->currentContainer().isNull()) {
        return QPoint(0, 0);
    }

    auto font = DisplayHelper::monoFont(10);
    auto container = m_handle->currentContainer();
    auto margin = DisplayHelper::textSize(font, "0").width() * 2;
    return  QPoint(
                DisplayHelper::textSize(font, container->frameCount()).width() + margin,
                DisplayHelper::textSize(font, container->maxFrameWidth()).width() + margin);
}
