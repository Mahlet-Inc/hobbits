#include "displaybasetext.h"

#include <math.h>
#include <QtMath>

#include "settingsmanager.h"
#include <QFontDatabase>
#include <QImage>
#include <QMenu>
#include <QPainter>

DisplayBaseText::DisplayBaseText(
    QSharedPointer<DisplayHandle> displayHandle,
    DisplayInterface *pluginRef,
    int columnGrouping) :
    DisplayBase(displayHandle, pluginRef),
    m_fontSize(10),
    m_columnGrouping(columnGrouping),
    m_showFrameOffsets(true),
    m_showColumnOffsets(true),
    m_displayOffset(0, 0),
    m_fontWidth(1),
    m_fontHeight(1),
    m_frameHeight(1)
{
}

void DisplayBaseText::paintEvent(QPaintEvent*)
{
    if (m_displayHandle->getContainer().isNull()) {
        return;
    }

    prepareHeaders();

    int w = qRound(double((this->width() - m_displayOffset.x()) * bitsPerChar()) / double(m_fontWidth));
    if (m_columnGrouping > 1) {
        w =
            qRound(
                    double((this->width() - m_displayOffset.x()) * bitsPerChar())
                    / (double(m_fontWidth) * double(m_columnGrouping + 1) / double(m_columnGrouping)));
    }
    int h = (height() - m_displayOffset.y()) / m_frameHeight;

    int frameOffset = m_displayHandle->getFrameOffset();
    int charOffset = qRound(double(m_displayHandle->getBitOffset()) / double(bitsPerChar()));


    if (frameOffset < 0) {
        return;
    }

    QPainter painter(this);
    QFont font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    font.setPointSize(m_fontSize);
    font.setStyleStrategy(QFont::ForceIntegerMetrics);
    if (m_showFrameOffsets) {
        painter.fillRect(0, 0, m_displayOffset.x() - m_fontWidth / 2, height(), Qt::lightGray);
        for (int i = 0; i < h; i++) {
            if (i + frameOffset >= m_displayHandle->getContainer()->frameCount()) {
                break;
            }
            painter.setPen(Qt::darkGray);
            painter.setFont(font);
            painter.drawText(
                    m_fontWidth / 2,
                    m_displayOffset.y() + i * m_frameHeight,
                    qRound(m_displayOffset.x() - (m_fontWidth * 1.5)),
                    m_frameHeight,
                    Qt::AlignRight,
                    QString("%1").arg(i + frameOffset));
        }
    }

    if (m_showColumnOffsets) {
        painter.fillRect(0, 0, width(), m_displayOffset.y() - m_fontWidth / 2, Qt::lightGray);
        for (int i = 0; i < w; i += 2) {
            if ((i + charOffset) * bitsPerChar() >= m_displayHandle->getContainer()->maxFrameWidth()) {
                break;
            }

            painter.save();
            painter.rotate(-90);
            painter.setPen(Qt::darkGray);

            int xOffset = qRound(getGroupedOffset(i, m_fontWidth, m_columnGrouping, charOffset, 1));
            painter.setPen(Qt::darkGray);
            painter.setFont(font);
            painter.drawText(
                    -1 * m_displayOffset.y() + m_fontWidth,
                    m_displayOffset.x() + xOffset - qFloor(double(m_fontWidth) / 2.5),
                    m_displayOffset.y(),
                    m_fontHeight,
                    Qt::AlignLeft,
                    QString("%1").arg((i + charOffset) * bitsPerChar()));
            painter.restore();
        }
    }

    painter.save();
    painter.translate(m_displayOffset);
    for (int i = 0; i < h; i++) {
        if (i + frameOffset >= m_displayHandle->getContainer()->frameCount()) {
            break;
        }
        Frame frame = m_displayHandle->getContainer()->frameAt(i + frameOffset);

        QString frameString = "";
        QString nibString;
        for (int ii = 0; ii < w; ii += bitsPerChar()) {
            int bitStart = ii + charOffset * bitsPerChar();
            if (bitStart >= frame.size()) {
                break;
            }

            frameString += getDisplayChars(frame, bitStart);

            if (m_columnGrouping > 1) {
                if (((charOffset * bitsPerChar() + ii) / bitsPerChar()) % m_columnGrouping == m_columnGrouping - 1) {
                    frameString += " ";
                }
            }
        }
        painter.setFont(font);
        painter.setPen(Qt::black);
        painter.drawText(
                0,
                i * m_frameHeight,
                (this->width() - m_displayOffset.x()),
                m_frameHeight,
                Qt::AlignLeft,
                frameString);
    }

    drawHighlights(
            &painter,
            double(m_fontWidth) / double(bitsPerChar()),
            m_frameHeight,
            frameOffset,
            charOffset * bitsPerChar(),
            w * bitsPerChar(),
            h,
            bitsPerChar() * m_columnGrouping,
            m_columnGrouping > 1 ? bitsPerChar() : 0);

    painter.restore();
}

void DisplayBaseText::mouseMoveEvent(QMouseEvent *event)
{
    sendHoverUpdate(
            event,
            m_fontWidth,
            m_frameHeight,
            m_columnGrouping,
            bitsPerChar(),
            m_displayOffset);
}

void DisplayBaseText::prepareHeaders()
{
    QPainter painter(this);
    QFont font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    font.setPointSize(m_fontSize);
    font.setStyleStrategy(QFont::ForceIntegerMetrics);
    QFontMetrics fontMetrics = QFontMetrics(font, painter.device());
    m_fontWidth = fontMetrics.width("0");
    m_fontHeight = fontMetrics.height();
    m_frameHeight = m_fontHeight + 2;

    if (m_showFrameOffsets) {
        qint64 totalFrames = m_displayHandle->getContainer()->frameCount();
        int maxChars = qFloor(log10(totalFrames)) + 1;
        m_displayOffset.setX(qRound(m_fontWidth * (maxChars + 1.5)));
    }
    else {
        m_displayOffset.setX(0);
    }

    if (m_showColumnOffsets) {
        qint64 maxWidth = m_displayHandle->getContainer()->maxFrameWidth();
        int maxChars = qCeil(log10(maxWidth));
        m_displayOffset.setY(qRound(m_fontWidth * (maxChars + 1.5)));
    }
    else {
        m_displayOffset.setY(0);
    }
}

void DisplayBaseText::adjustScrollbars()
{
    if (!m_displayHandle->getCurrentFocusDisplays().contains(m_pluginRef)) {
        return;
    }
    if (m_displayHandle->getContainer().isNull()) {
        DisplayBase::adjustScrollbars();
        return;
    }

    repaint();

    int w = qRound(double((this->width() - m_displayOffset.x()) * bitsPerChar()) / double(m_fontWidth));
    if (m_columnGrouping > 1) {
        w =
            qRound(
                    double((this->width() - m_displayOffset.x()) * bitsPerChar())
                    / (double(m_fontWidth) * double(m_columnGrouping + 1) / double(m_columnGrouping)));
    }
    int h = (height() - m_displayOffset.y()) / m_frameHeight;

    DisplayBase::adjustScrollbars();

    if (m_displayHandle->getCurrentFocusDisplays().size() == 1) {
        m_displayHandle->getVScroll()->setPageStep(h);
        m_displayHandle->getHScroll()->setPageStep(w);
        m_displayHandle->getHScroll()->setSingleStep(bitsPerChar());
        m_displayHandle->getHScroll()->setMaximum(m_displayHandle->getContainer()->maxFrameWidth() - bitsPerChar());
    }
}

void DisplayBaseText::setFontSize(int fontSize)
{
    m_fontSize = fontSize;
    adjustScrollbars();
}

void DisplayBaseText::setColumnGrouping(int columnGrouping)
{
    m_columnGrouping = columnGrouping;
    adjustScrollbars();
}

void DisplayBaseText::setShowHeaders(bool showHeaders)
{
    m_showFrameOffsets = showHeaders;
    m_showColumnOffsets = showHeaders;
    adjustScrollbars();
}
