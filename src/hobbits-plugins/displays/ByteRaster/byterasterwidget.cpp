#include "byterasterwidget.h"

#include "displayhelper.h"
#include "settingsmanager.h"
#include <math.h>
#include <QImage>
#include <QPainter>
#include <QtMath>

ByteRasterWidget::ByteRasterWidget(QSharedPointer<DisplayHandle> displayHandle, DisplayInterface *pluginRef) :
    DisplayBase(displayHandle, pluginRef),
    m_scale(3),
    m_showFrameOffsets(true),
    m_showColumnOffsets(true),
    m_displayOffset(0, 0),
    m_headerFontSize(0, 0)
{

}

void ByteRasterWidget::paintEvent(QPaintEvent*)
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
            if (i + m_displayHandle->getFrameOffset() >= m_displayHandle->getContainer()->frames().size()) {
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
            if (i * 8 + m_displayHandle->getBitOffset() >= m_displayHandle->getContainer()->maxFrameWidth()) {
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
                    QString("%1").arg(i * 8 + m_displayHandle->getBitOffset()));
            painter.restore();
        }
    }
    painter.restore();

    QImage raster = DisplayHelper::getByteRasterImage(
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

    int byteOffset = m_displayHandle->getBitOffset() / 8;
    int adjustedBitOffset = byteOffset * 8;

    painter.resetTransform();
    painter.translate(m_displayOffset);
    drawHighlights(
            &painter,
            double(m_scale) / 8.0,
            m_scale,
            m_displayHandle->getFrameOffset(),
            adjustedBitOffset,
            displayWidth * 8,
            displayHeight,
            1);

    painter.restore();
}

void ByteRasterWidget::mouseMoveEvent(QMouseEvent *event)
{
    sendHoverUpdate(event, m_scale, m_scale, 1, 8, m_displayOffset);
}

void ByteRasterWidget::prepareHeaders()
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
        int totalFrames = m_displayHandle->getContainer()->frames().size();
        int maxChars = qFloor(log10(totalFrames)) + 1;
        m_displayOffset.setX(qRound(fontWidth * (maxChars + 1.5)));
    }
    else {
        m_displayOffset.setX(0);
    }

    if (m_showColumnOffsets) {
        int maxWidth = m_displayHandle->getContainer()->maxFrameWidth();
        int maxChars = qFloor(log10(maxWidth)) + 1;
        m_displayOffset.setY(fontWidth * (maxChars + 1));
    }
    else {
        m_displayOffset.setY(0);
    }
}

void ByteRasterWidget::adjustScrollbars()
{
    if (!m_displayHandle->getCurrentFocusDisplays().contains(m_pluginRef)) {
        return;
    }
    DisplayBase::adjustScrollbars();

    repaint();

    if (m_displayHandle->getCurrentFocusDisplays().size() == 1) {
        m_displayHandle->getHScroll()->setSingleStep(8);
        m_displayHandle->getVScroll()->setPageStep(8 * (height() - m_displayOffset.y()) / m_scale);
        m_displayHandle->getHScroll()->setPageStep(8 * (width() - m_displayOffset.x()) / m_scale);
    }
}

void ByteRasterWidget::setScale(int scale)
{
    m_scale = scale;
    this->adjustScrollbars();
}

void ByteRasterWidget::setShowHeaders(bool showHeaders)
{
    m_showFrameOffsets = showHeaders;
    m_showColumnOffsets = showHeaders;
    adjustScrollbars();
}
