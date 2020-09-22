#include "peakselector.h"
#include <cmath>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>

PeakSelector::PeakSelector(QWidget *parent) :
    QWidget(parent),
    m_dataYMax(0),
    m_dataYMin(0),
    m_startDrag(-1),
    m_endDrag(-1),
    m_disabled(true),
    m_hasNan(false)
{
    this->setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::MinimumExpanding);
    m_hScroll = new QScrollBar(Qt::Horizontal, this);

    connect(m_hScroll, SIGNAL(valueChanged(int)), this, SLOT(repaint()));

    m_zoomSlider = new QSlider(Qt::Horizontal, this);
    m_zoomSlider->setValue(5);
    m_zoomSlider->setMinimum(1);
    m_zoomSlider->setMaximum(600);

    m_zoomSlider->setInvertedControls(true);

    connect(m_zoomSlider, SIGNAL(valueChanged(int)), this, SLOT(adjustScroll()));

    this->setMouseTracking(true);
}

QSlider* PeakSelector::getZoomSlider()
{
    return m_zoomSlider;
}

QScrollBar* PeakSelector::getHScroll()
{
    return m_hScroll;
}

void PeakSelector::adjustScroll()
{
    m_hScroll->setMinimum(0);
    m_hScroll->setMaximum(m_data.size() / m_zoomSlider->value());

    repaint();
}

void PeakSelector::paintEvent(QPaintEvent*)
{
    QPainter painter(this);

    painter.fillRect(0, 0, this->width(), this->height(), Qt::white);

    if (m_data.isEmpty()) {
        return;
    }

    if (m_hasNan) {
        painter.drawText(5, this->height() / 2, "Error: Data from autocorrelation contains NaN.");
        return;
    }

    double yOffset = -1 * m_dataYMin;
    double yScale = (this->height() * .9) / (m_dataYMax - m_dataYMin);

    int xOffset = 1 + m_hScroll->value() * m_zoomSlider->value();


    QPainterPath path;
    if (xOffset < m_data.size()) {
        double yValue = (m_data.at(xOffset).y() + yOffset) * yScale;
        yValue = this->height() - yValue;
        path.moveTo(0, yValue);
    }
    for (int i = 0; i < this->width() && i * m_zoomSlider->value() + xOffset < m_data.size(); i++) {
        int start = i * m_zoomSlider->value();
        double min = m_data.at(xOffset + start).y();
        double max = m_data.at(xOffset + start).y();
        for (int ii = 1; ii < m_zoomSlider->value() && xOffset + start + ii < m_data.size(); ii++) {
            min = qMin(min, m_data.at(xOffset + start + ii).y());
            max = qMax(max, m_data.at(xOffset + start + ii).y());
        }
        double minY = this->height() - (min + yOffset) * yScale;
        double maxY = this->height() - (max + yOffset) * yScale;
        path.lineTo(i + 1, minY);
        path.lineTo(i + 1, maxY);
    }

    QRgb lineColor = qRgb(30, 40, 180);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setPen(lineColor);
    painter.drawPath(path);

    QFont font = QFont("monospace", 11);
    font.setStyleStrategy(QFont::ForceIntegerMetrics);
    QFontMetrics fontMetrics = QFontMetrics(font, painter.device());

    QFont bigFont = QFont("monospace", 12);
    bigFont.setStyleStrategy(QFont::ForceIntegerMetrics);
    QFontMetrics bigFontMetrics = QFontMetrics(bigFont, painter.device());

    int textPad = 2;
    if (m_startDrag >= 0) {
        QPointF peak = getPeak();
        if (!peak.isNull()) {
            double yValue = (peak.y() + yOffset) * yScale;
            yValue = this->height() - yValue;
            painter.setBrush(QBrush(lineColor));
            QPointF peakPoint = QPointF((peak.x() - xOffset) / m_zoomSlider->value(), yValue);
            painter.drawEllipse(peakPoint + QPointF(1, 0), 2, 2);

            painter.setOpacity(0.85);
            painter.setPen(Qt::black);
            painter.setFont(bigFont);
            QString valString = QString("%1").arg(peak.x());
            int textWidth = bigFontMetrics.width(valString);
            int textX = int(peakPoint.x() - textWidth/2);
            if (textX < textPad) {
                textX = textPad;
            }
            else if (textX + textWidth + textPad > this->width()) {
                textX = this->width() - textWidth - textPad;
            }
            painter.drawText(textX, int(peakPoint.y() - textPad - 3), valString);
        }

        painter.setOpacity(0.15);
        painter.fillRect(m_startDrag, 0, m_endDrag - m_startDrag, this->height(), Qt::black);
    }

    if (!m_hover.isNull()) {
        painter.setOpacity(0.70);
        painter.setPen(Qt::black);
        painter.drawLine(m_hover.x(), 0, m_hover.x(), this->height());

        int location = (m_hover.x() * m_zoomSlider->value()) + m_hScroll->value();
        if (location >= 0 && location < m_data.size()) {
            QString valString = QString("%1").arg(location);
            painter.setFont(font);
            int textWidth = fontMetrics.width(valString);
            int textHeight = fontMetrics.height();
            int textX = m_hover.x() + textPad;
            if (textWidth + textX > this->width() - textPad) {
                textX = m_hover.x() - textPad - textWidth;
            }
            painter.drawText(textX, textHeight + textPad, valString);
        }

    }
}

QPointF PeakSelector::getPeak()
{
    if (m_startDrag < 0) {
        return QPointF();
    }

    int start = m_startDrag + m_hScroll->value();
    int end = m_endDrag + m_hScroll->value();

    if (start > end) {
        start = end;
        end = m_startDrag + m_hScroll->value();
    }

    start *= m_zoomSlider->value();
    end *= m_zoomSlider->value();

    if (start < 0) {
        return QPointF();
    }

    if (start >= m_data.size()) {
        return QPointF();
    }

    if (end >= m_data.size()) {
        end = m_data.size() - 1;
    }

    int maxIdx = start;
    double maxY = m_data.at(start).y();
    for (int i = start + 1; i < end; i++) {
        if (m_data.at(i).y() > maxY) {
            maxIdx = i;
            maxY = m_data.at(i).y();
        }
    }

    return (QPointF(maxIdx, maxY));
}

void PeakSelector::setData(QVector<QPointF> data)
{
    m_hasNan = false;

    m_data = data;
    m_dataYMin = 0;
    m_dataYMax = 0;
    for (QPointF point : m_data) {
        if (point.y() < m_dataYMin) {
            m_dataYMin = point.y();
        }
        else if (point.y() > m_dataYMax) {
            m_dataYMax = point.y();
        }
        if (std::isnan(point.y())) {
            m_hasNan = true;
        }
    }

    m_disabled = m_hasNan;

    adjustScroll();
}

void PeakSelector::mouseMoveEvent(QMouseEvent *event)
{
    if (m_disabled) {
        return;
    }
    if (m_startDrag >= 0) {
        m_endDrag = event->x();
    }
    m_hover = event->pos();
    this->repaint();
}

void PeakSelector::mousePressEvent(QMouseEvent *event)
{
    if (m_disabled) {
        return;
    }
    m_startDrag = event->x();
    m_endDrag = event->x();
}

void PeakSelector::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_disabled) {
        return;
    }
    m_endDrag = event->x();

    QPointF peak = getPeak();

    m_startDrag = -1;
    m_endDrag = -1;

    if (!peak.isNull()) {
        emit peakSelected(peak);
    }

    this->repaint();
}

void PeakSelector::leaveEvent(QEvent * event)
{
    Q_UNUSED(event)
    if (m_disabled) {
        return;
    }
    m_hover = QPoint();
    this->repaint();
}
