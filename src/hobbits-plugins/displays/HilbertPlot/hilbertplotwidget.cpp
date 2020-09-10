#include "hilbertplotwidget.h"

#include <QPainter>
#include <QImage>
#include "displayhelper.h"

HilbertPlotWidget::HilbertPlotWidget(
        QSharedPointer<DisplayHandle> displayHandle,
        DisplayInterface *pluginRef,
        QWidget *parent) :
    DisplayBase(displayHandle, pluginRef, parent)
{
    for (int i = 1; i < 6; i++) {
        m_hilbertPoints.insert(i, getPointsForOrder(i));
    }
    m_hilbertRecursionOrders = {4, 4, 2};
}

void HilbertPlotWidget::paintEvent(QPaintEvent*) {
    if (m_displayHandle->getContainer().isNull()) {
        return;
    }
    if (m_displayHandle->getFrameOffset() >= m_displayHandle->getContainer()->info()->frameCount()) {
        return;
    }

    int rasterLength = 1;
    for (int order : m_hilbertRecursionOrders) {
        rasterLength *= 1 << order;
    }

    QImage raster(rasterLength, rasterLength, QImage::Format_ARGB32);
    raster.fill(qRgba(0, 0, 0, 0));

    auto bits = m_displayHandle->getContainer()->bits();
    qint64 byteOffset = m_displayHandle->getContainer()->info()->frames()->at(m_displayHandle->getFrameOffset()).start() / 8;
    for (int i = 0; i + byteOffset < bits->sizeInBytes() && i < rasterLength * rasterLength; i++) {
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
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.drawImage(0, 0, raster);
}

void HilbertPlotWidget::mouseMoveEvent(QMouseEvent *event) {
    sendHoverUpdate(event, 1, 1, 1, 1, QPoint(0, 0));
}

void HilbertPlotWidget::rotate(QPoint &p, int n, bool rx, bool ry) {
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

QPoint HilbertPlotWidget::toHilbertCoordinate(int n, int idx) {
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

QVector<QPoint> HilbertPlotWidget::getPointsForOrder(int order) {
    QVector<QPoint> points;
    int n = 1 << order;
    for (int d = 0; d < n * n; ++d) {
        points.push_back(toHilbertCoordinate(n, d));
    }
    return points;
}
