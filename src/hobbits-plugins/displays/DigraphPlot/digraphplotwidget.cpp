#include "digraphplotwidget.h"

#include <QPainter>
#include <QImage>
#include "displayhelper.h"

DigraphPlotWidget::DigraphPlotWidget(
        QSharedPointer<DisplayHandle> displayHandle,
        DisplayInterface *pluginRef,
        QWidget *parent) :
    DisplayBase(displayHandle, pluginRef, parent),
    m_windowSize(10000),
    m_wordSize(8),
    m_scale(2)
{

}

void DigraphPlotWidget::paintEvent(QPaintEvent*) {
    if (m_displayHandle->getContainer().isNull()) {
        return;
    }

    auto bits = m_displayHandle->getContainer()->bits();
    auto frames = m_displayHandle->getContainer()->bitInfo()->frames();
    auto frameOffset = m_displayHandle->getFrameOffset();
    if (frames->size() <= frameOffset) {
        return;
    }
    qint64 startBit = (frames->at(frameOffset).start() / m_wordSize) * m_wordSize;

    int plotSize = 1 << m_wordSize;
    QRgb background = qRgb(0, 0, 0);
    QRgb foreground = qRgb(200, 230, 255);
    QImage digraphPlot(plotSize, plotSize, QImage::Format_ARGB32);
    digraphPlot.fill(background);

    int lastWord = 0;
    int nextWord = 0;
    for (qint64 word = 0; word < m_windowSize; word++) {
        qint64 start = startBit + word * m_wordSize;
        qint64 end = start + m_wordSize;
        if (bits->sizeInBits() < end) {
            break;
        }
        lastWord = nextWord;
        nextWord = int(bits->getWordValue(start, int(m_wordSize)));
        if (word == 0) {
            continue;
        }
        digraphPlot.setPixel(lastWord, nextWord, foreground);
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.scale(m_scale, m_scale);
    painter.drawImage(0, 0, digraphPlot);
}

void DigraphPlotWidget::mouseMoveEvent(QMouseEvent *event) {
    Q_UNUSED(event)
}

void DigraphPlotWidget::setWindowSize(int size)
{
    m_windowSize = size;
    adjustScrollbars();
}

void DigraphPlotWidget::setWordSize(int size)
{
    m_wordSize = size;
    adjustScrollbars();
}

void DigraphPlotWidget::setScale(int size)
{
    m_scale = size;
    adjustScrollbars();
}

void DigraphPlotWidget::adjustScrollbars()
{
    if (!m_displayHandle->getCurrentFocusDisplays().contains(m_pluginRef)) {
        return;
    }
    DisplayBase::adjustScrollbars();

    repaint();

    if (m_displayHandle->getCurrentFocusDisplays().size() == 1) {
        m_displayHandle->getHScroll()->setVisible(false);
        m_displayHandle->getHScroll()->setEnabled(false);
    }
}
