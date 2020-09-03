#include "symbolrasterwidget.h"

#include "settingsmanager.h"
#include <QImage>
#include <QPainter>
#include <QtMath>

SymbolRasterWidget::SymbolRasterWidget(QSharedPointer<DisplayHandle> displayHandle, DisplayInterface *pluginRef) :
    DisplayBase(displayHandle, pluginRef),
    m_scale(3),
    m_showFrameOffsets(true),
    m_showColumnOffsets(true),
    m_displayOffset(0, 0),
    m_headerFontSize(0, 0),
    m_symbolLength(1)
{

}

QImage SymbolRasterWidget::getSymbolMapImage(
        QSharedPointer<BitContainer> bitContainer,
        int x,
        int y,
        int w,
        int h) const
{
    QImage raster(w, h, QImage::Format_ARGB32);
    raster.fill(qRgba(0x66, 0x66, 0x66, 0x66));

    int frameOffset = y;
    int symbolOffset = x / m_symbolLength;
    int bitOffset = symbolOffset * m_symbolLength;

    if (frameOffset < 0) {
        return raster;
    }

    for (int i = 0; i < h; i++) {
        if (i + frameOffset >= bitContainer->frameCount()) {
            break;
        }
        Frame frame = bitContainer->frameAt(i + frameOffset);

        for (int ii = 0; ii < w * m_symbolLength; ii += m_symbolLength) {
            if (ii + bitOffset + m_symbolLength > frame.size()) {
                break;
            }

            quint64 symbolVal = 0;
            for (int bit = m_symbolLength - 1; bit >= 0; bit--) {
                if (frame.at(ii + bit + bitOffset)) {
                    symbolVal |= 0x01 << (m_symbolLength - bit - 1);
                }
            }

            if (m_colorMapping.contains(symbolVal)) {
                raster.setPixel(ii / m_symbolLength, i, m_colorMapping.value(symbolVal).rgba());
            }

        }
    }
    return raster;
}

void SymbolRasterWidget::paintEvent(QPaintEvent*)
{
    if (m_displayHandle->getContainer().isNull()) {
        return;
    }

    prepareHeaders();

    int displayWidth = (this->width() - m_displayOffset.x()) / m_scale;
    int displayHeight = (this->height() - m_displayOffset.y()) / m_scale;


    QPainter painter(this);

    painter.save();
    if (m_showFrameOffsets) {
        painter.fillRect(0, 0, m_displayOffset.x(), height(), Qt::lightGray);
    }
    if (m_showColumnOffsets) {
        painter.fillRect(0, 0, width(), m_displayOffset.y(), Qt::lightGray);
    }

    QFont font = QFont("monospace", 10);
    font.setStyleStrategy(QFont::ForceIntegerMetrics);
    if (m_showFrameOffsets) {
        int increment = qCeil(double(m_headerFontSize.height()) / double(m_scale));
        for (int i = 0; i <= displayHeight; i += increment) {
            if (i + m_displayHandle->getFrameOffset() >= m_displayHandle->getContainer()->frameCount()) {
                break;
            }

            int yOffset = -2;

            if (increment > 1) {
                yOffset = -1 * qRound(double(m_headerFontSize.height() - m_scale) / 2.0);
                painter.fillRect(
                        m_displayOffset.x() - m_headerFontSize.width() / 2,
                        m_displayOffset.y() + (i * m_scale),
                        m_headerFontSize.width() / 2,
                        m_scale,
                        Qt::darkGray);
            }

            painter.setPen(Qt::darkGray);
            painter.setFont(font);
            painter.drawText(
                    m_headerFontSize.width() / 2,
                    m_displayOffset.y() + (i * m_scale) + yOffset,
                    m_displayOffset.x() - m_headerFontSize.width(),
                    m_headerFontSize.height(),
                    Qt::AlignRight | Qt::AlignTop,
                    QString("%1").arg(i + m_displayHandle->getFrameOffset()));
        }
    }
    painter.restore();

    painter.save();
    if (m_showColumnOffsets) {
        int increment = qCeil(double(m_headerFontSize.height()) / double(m_scale));
        for (int i = 0; i < displayWidth; i += increment) {
            if (i * m_symbolLength + m_displayHandle->getBitOffset()
                >= m_displayHandle->getContainer()->maxFrameWidth()) {
                break;
            }

            int yOffset = -2;

            if (increment > 1) {
                yOffset = -1 * qRound(double(m_headerFontSize.height() - m_scale) / 2.0);
                painter.fillRect(
                        m_displayOffset.x() + i * m_scale,
                        m_displayOffset.y() - m_headerFontSize.width() / 2,
                        m_scale,
                        m_headerFontSize.width() / 2,
                        Qt::darkGray);
            }

            painter.save();
            painter.rotate(-90);
            painter.setPen(Qt::darkGray);
            painter.setFont(font);
            painter.drawText(
                    -1 * m_displayOffset.y() + m_headerFontSize.width() / 2,
                    m_displayOffset.x() + i * m_scale + yOffset,
                    m_displayOffset.y() - m_headerFontSize.width(),
                    m_headerFontSize.height(),
                    Qt::AlignLeft,
                    QString("%1").arg(i * m_symbolLength + m_displayHandle->getBitOffset()));
            painter.restore();
        }
    }
    painter.restore();

    QImage raster = this->getSymbolMapImage(
            m_displayHandle->getContainer(),
            m_displayHandle->getBitOffset(),
            m_displayHandle->getFrameOffset(),
            displayWidth,
            displayHeight);

    painter.save();
    painter.translate(m_displayOffset);
    painter.scale(m_scale, m_scale);
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.drawImage(0, 0, raster);

    int symbolOffset = m_displayHandle->getBitOffset() / m_symbolLength;
    int adjustedBitOffset = symbolOffset * m_symbolLength;

    painter.resetTransform();
    painter.translate(m_displayOffset);
    drawHighlights(
            &painter,
            double(m_scale) / double(m_symbolLength),
            m_scale,
            m_displayHandle->getFrameOffset(),
            adjustedBitOffset,
            displayWidth * m_symbolLength,
            displayHeight,
            1);

    painter.restore();
}

void SymbolRasterWidget::mouseMoveEvent(QMouseEvent *event)
{
    sendHoverUpdate(event, m_scale, m_scale, 1, m_symbolLength, m_displayOffset);
}

void SymbolRasterWidget::prepareHeaders()
{
    if (m_displayHandle->getContainer().isNull()) {
        return;
    }

    QPainter painter(this);
    QFont font = QFont("monospace", 10);
    font.setStyleStrategy(QFont::ForceIntegerMetrics);
    QFontMetrics fontMetrics = QFontMetrics(font, painter.device());
    int fontWidth = fontMetrics.width("0");
    int fontHeight = fontMetrics.height();
    m_headerFontSize.setWidth(fontWidth);
    m_headerFontSize.setHeight(fontHeight);

    if (m_showFrameOffsets) {
        int totalFrames = m_displayHandle->getContainer()->frameCount();
        int maxChars = qFloor(log10(totalFrames)) + 1;
        m_displayOffset.setX(qRound(fontWidth * (maxChars + 1.5)));
    }
    else {
        m_displayOffset.setX(0);
    }

    if (m_showColumnOffsets) {
        qint64 maxWidth = m_displayHandle->getContainer()->maxFrameWidth();
        int maxChars = qFloor(log10(maxWidth)) + 1;
        m_displayOffset.setY(fontWidth * (maxChars + 1));
    }
    else {
        m_displayOffset.setY(0);
    }
}

void SymbolRasterWidget::adjustScrollbars()
{
    if (!m_displayHandle->getCurrentFocusDisplays().contains(m_pluginRef)) {
        return;
    }
    DisplayBase::adjustScrollbars();

    repaint();

    if (m_displayHandle->getCurrentFocusDisplays().size() == 1) {
        m_displayHandle->getHScroll()->setSingleStep(m_symbolLength);
        m_displayHandle->getVScroll()->setPageStep((height() - m_displayOffset.y()) / m_scale);
        m_displayHandle->getHScroll()->setPageStep(m_symbolLength * (width() - m_displayOffset.x()) / m_scale);
    }
}

void SymbolRasterWidget::setScale(int scale)
{
    m_scale = scale;
    this->adjustScrollbars();
}

void SymbolRasterWidget::setShowHeaders(bool showHeaders)
{
    m_showFrameOffsets = showHeaders;
    m_showColumnOffsets = showHeaders;
    adjustScrollbars();
}

void SymbolRasterWidget::setColorMapping(QList<QPair<QString, QColor>> mapping)
{
    m_colorMapping.clear();
    if (mapping.length() < 1) {
        return;
    }

    m_symbolLength = mapping.at(0).first.length();

    for (QPair<QString, QColor> pair : mapping) {
        bool ok = false;
        quint64 symbolValue = pair.first.toULongLong(&ok, 2);
        if (ok) {
            m_colorMapping.insert(symbolValue, pair.second);
        }
    }

    adjustScrollbars();
}
