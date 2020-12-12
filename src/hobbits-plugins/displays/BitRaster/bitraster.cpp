#include "bitraster.h"
#include "bitrastercontrols.h"
#include "displayhelper.h"
#include <QPainter>
#include <QtMath>

BitRaster::BitRaster() :
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
                            return QString("Bit Raster %1x with headers").arg(scale);
                        }
                        else {
                            return QString("Bit Raster %1x without headers").arg(scale);
                        }
                    },
                    [](QSharedPointer<ParameterDelegate> delegate, QSize size) {
                        Q_UNUSED(size)
                        return new BitRasterControls(delegate);
                    });
}

DisplayInterface* BitRaster::createDefaultDisplay()
{
    return new BitRaster();
}

QString BitRaster::name()
{
    return "Bit Raster";
}

QString BitRaster::description()
{
    return "Displays each bit in the data as a single colored pixel";
}

QStringList BitRaster::tags()
{
    return {"Generic"};
}

QSharedPointer<DisplayRenderConfig> BitRaster::renderConfig()
{
    return m_renderConfig;
}

void BitRaster::setDisplayHandle(QSharedPointer<DisplayHandle> displayHandle)
{
    m_handle = displayHandle;
    DisplayHelper::connectHoverUpdates(this, this, m_handle, [this](QPoint& offset, QSize &symbolSize, int &grouping, int &bitsPerSymbol) {
        if (!m_delegate->validate(m_lastParams)) {
            return false;
        }
        int scale = m_lastParams.value("scale").toInt();
        offset = headerOffset(m_lastParams);
        symbolSize = QSize(scale, scale);
        grouping = 1;
        bitsPerSymbol = 1;
        return true;
    });
}

QSharedPointer<ParameterDelegate> BitRaster::parameterDelegate()
{
    return m_delegate;
}

QImage BitRaster::renderDisplay(QSize viewportSize, const QJsonObject &parameters, QSharedPointer<PluginActionProgress> progress)
{
    Q_UNUSED(progress)
    m_lastParams = parameters;
    if (m_handle.isNull() || m_handle->currentContainer().isNull() || !m_delegate->validate(m_lastParams)) {
        return QImage();
    }

    int scale = parameters.value("scale").toInt();
    QPoint offset = headerOffset(parameters);
    QSize rasterSize(viewportSize.width() - offset.x(), viewportSize.height() - offset.y());
    QSize sourceSize(qMax(1, rasterSize.width() / scale), qMax(1, rasterSize.height() / scale));

    QImage raster = DisplayHelper::getBitRasterImage(
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
    DisplayHelper::drawHighlights(
                m_handle,
                &painter,
                QSize(scale, scale),
                sourceSize,
                m_handle->bitOffset(),
                m_handle->frameOffset(),
                1);

    DisplayHelper::setRenderRange(this, m_handle, sourceSize.height());

    return destImage;
}

QImage BitRaster::renderOverlay(QSize viewportSize, const QJsonObject &parameters)
{
    m_lastParams = parameters;
    if (!m_delegate->validate(m_lastParams)) {
        return QImage();
    }
    int scale = parameters.value("scale").toInt();

    return DisplayHelper::drawHeadersFull(
                viewportSize,
                headerOffset(parameters),
                m_handle,
                QSizeF(scale, scale));
}

QPoint BitRaster::headerOffset(const QJsonObject &parameters)
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
