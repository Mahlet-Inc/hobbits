#include "hilbertplot.h"
#include "hilbertplotform.h"
#include "displayresult.h"
#include <QPainter>

HilbertPlot::HilbertPlot() :
    m_renderConfig(new DisplayRenderConfig())
{
    m_renderConfig->setFullRedrawTriggers(DisplayRenderConfig::NewFrameOffset);
    m_renderConfig->setAsynchronous(true);
    m_renderConfig->setHideBitOffsetControls(true);

    QList<ParameterDelegate::ParameterInfo> infos = {
    };

    m_delegate = ParameterDelegate::create(
                infos,
                [this](const QJsonObject &parameters) {
        Q_UNUSED(parameters)
        return QString("Hilbert Plot").arg(this->name());
    },
    [](QSharedPointer<ParameterDelegate> delegate, QSize size) {
            Q_UNUSED(size)
            return new HilbertPlotForm(delegate);
});

    for (int i = 1; i < 6; i++) {
        m_hilbertPoints.insert(i, getPointsForOrder(i));
    }
    m_hilbertRecursionOrders = {4, 4, 2};
}

DisplayInterface* HilbertPlot::createDefaultDisplay()
{
    return new HilbertPlot();
}

QString HilbertPlot::name()
{
    return "Hilbert Plot";
}

QString HilbertPlot::description()
{
    return "Displays byte-based pixels in a fractal pattern to highlight chunks in heterogeneous data";
}

QStringList HilbertPlot::tags()
{
    return {"Generic"};
}

QSharedPointer<DisplayRenderConfig> HilbertPlot::renderConfig()
{
    return m_renderConfig;
}

void HilbertPlot::setDisplayHandle(QSharedPointer<DisplayHandle> displayHandle)
{
    m_handle = displayHandle;
}

QSharedPointer<ParameterDelegate> HilbertPlot::parameterDelegate()
{
    return m_delegate;
}

QSharedPointer<DisplayResult> HilbertPlot::renderDisplay(QSize viewportSize, const QJsonObject &parameters, QSharedPointer<PluginActionProgress> progress)
{
    Q_UNUSED(parameters)

    QStringList invalidations = m_delegate->validate(parameters);
    if (!invalidations.isEmpty()) {
        return DisplayResult::error(QString("Invalid parameters passed to %1:\n%2").arg(name()).arg(invalidations.join("\n")));
    }
    if (m_handle.isNull() || m_handle->currentContainer().isNull()) {
        return DisplayResult::nullResult();
    }

    int rasterLength = 1;
    for (int order : m_hilbertRecursionOrders) {
        rasterLength *= 1 << order;
    }

    QImage raster(viewportSize, QImage::Format_ARGB32);
    raster.fill(Qt::transparent);

    auto bits = m_handle->currentContainer()->bits();
    qint64 byteOffset = m_handle->currentContainer()->info()->frames()->at(m_handle->frameOffset()).start() / 8;
    QTime lastTime = QTime::currentTime();
    for (int i = 0; i + byteOffset < bits->sizeInBytes() && i < rasterLength * rasterLength; i++) {
        if (progress->isCancelled()) {
            return DisplayResult::error("Render Cancelled");
        }
        int chunkLength = 1;
        int chunk = i;
        QPoint p(0, 0);
        for (int order : m_hilbertRecursionOrders) {
            auto points = m_hilbertPoints.value(order);
            QPoint pChunk = points.at(chunk % points.size());
            p.setX(p.x() + (pChunk.x() * chunkLength));
            p.setY(p.y() + (pChunk.y() * chunkLength));
            chunk = chunk / points.size();
            chunkLength *= 1 << order;
        }
        if (p.x() >= viewportSize.width() || p.y() >= viewportSize.height()) {
            continue;
        }
        char value = bits->byteAt(i+byteOffset);
        if (value == 0x00) {
            raster.setPixel(p, qRgba(0, 0, 0, 255));
        }
        else if (value == -128) {
            raster.setPixel(p, qRgba(255, 255, 255, 255));
        }
        else if (isascii(value)) {
            raster.setPixel(p, qRgba(100, 160, 255, 255));
        }
        else {
            raster.setPixel(p, qRgba(255, 130, 130, 255));
        }
        if (i % 8192 == 0) {
            if ( !progress.isNull()
                 && !progress->isCancelled()
                 && lastTime.msecsTo(QTime::currentTime()) > 250) {
                QImage preview(viewportSize, QImage::Format_ARGB32);
                preview.fill(Qt::transparent);
                QPainter previewPaint(&preview);
                previewPaint.drawImage(0, 0, raster, 0, 0, rasterLength, rasterLength);
                progress->sendUpdate("image_preview", QVariant(preview));
                progress->setProgress(i, rasterLength * rasterLength);
                lastTime = QTime::currentTime();
            }
        }
    }

    return DisplayResult::result(raster, parameters);
}

QSharedPointer<DisplayResult> HilbertPlot::renderOverlay(QSize viewportSize, const QJsonObject &parameters)
{
    Q_UNUSED(viewportSize)
    Q_UNUSED(parameters)
    QStringList invalidations = m_delegate->validate(parameters);
    if (!invalidations.isEmpty()) {
        m_handle->setRenderedRange(this, Range());
        return DisplayResult::error(QString("Invalid parameters passed to %1:\n%2").arg(name()).arg(invalidations.join("\n")));
    }
    if (m_handle.isNull() || m_handle->currentContainer().isNull()) {
        m_handle->setRenderedRange(this, Range());
        return DisplayResult::nullResult();
    }

    int rasterLength = 1;
    for (int order : m_hilbertRecursionOrders) {
        rasterLength *= 1 << order;
    }

    qint64 byteOffset = m_handle->currentContainer()->info()->frames()->at(m_handle->frameOffset()).start() / 8;
    qint64 lastByte = byteOffset + (rasterLength * rasterLength);
    qint64 lastBit = qMin(m_handle->currentContainer()->bits()->sizeInBits() - 1, lastByte * 8);
    m_handle->setRenderedRange(this, Range(byteOffset * 8, lastBit));

    return DisplayResult::nullResult();
}

void HilbertPlot::rotate(QPoint &p, int n, bool rx, bool ry) {
    if (!ry) {
        if (rx) {
            p.setX((n - 1) - p.x());
            p.setY((n - 1) - p.y());
        }
        int x = p.y();
        p.setY(p.x());
        p.setX(x);
    }
}

QPoint HilbertPlot::toHilbertCoordinate(int n, int idx) {
    QPoint p(0, 0);
    bool rx, ry;
    int t = idx;
    for (int s = 1; s < n; s <<= 1) {
        rx = ((t & 2) != 0);
        ry = (((t ^ (rx ? 1 : 0)) & 1) != 0);
        rotate(p, s, rx, ry);
        p.setX(p.x() + (rx ? s : 0));
        p.setY(p.y() + (ry ? s : 0));
        t >>= 2;
    }
    return p;
}

QVector<QPoint> HilbertPlot::getPointsForOrder(int order) {
    QVector<QPoint> points;
    int n = 1 << order;
    for (int d = 0; d < n * n; ++d) {
        points.push_back(toHilbertCoordinate(n, d));
    }
    return points;
}
