#include "dotplotwidget.h"

#include <QPainter>
#include <QImage>
#include "displayhelper.h"

DotPlotWidget::DotPlotWidget(
        QSharedPointer<DisplayHandle> displayHandle,
        DisplayInterface *pluginRef,
        QWidget *parent) :
    DisplayBase(displayHandle, pluginRef, parent),
    m_windowSize(512),
    m_wordSize(8),
    m_scale(1)
{

}

void DotPlotWidget::paintEvent(QPaintEvent*) {
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

    QRgb background = qRgb(0, 0, 0);
    QRgb foreground = qRgb(200, 230, 255);
    QImage dotPlot(m_windowSize, m_windowSize, QImage::Format_ARGB32);
    dotPlot.fill(background);

    QVector<int> words(m_windowSize);
    for (int word = 0; word < m_windowSize; word++) {
        qint64 start = startBit + word * m_wordSize;
        qint64 end = start + m_wordSize;
        if (bits->sizeInBits() < end) {
            words.resize(word);
            break;
        }
        words[word] = int(bits->getWordValue(start, int(m_wordSize)));
    }
    for (int x = 0; x < words.size(); x++) {
        for (int y = 0; y < words.size(); y++) {
            if (words.at(x) == words.at(y)) {
                dotPlot.setPixel(x, y, foreground);
            }
        }
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.scale(m_scale, m_scale);
    painter.drawImage(0, 0, dotPlot);
}

void DotPlotWidget::mouseMoveEvent(QMouseEvent *event) {
    Q_UNUSED(event)
}

void DotPlotWidget::setWindowSize(int size)
{
    m_windowSize = size;
    adjustScrollbars();
}

void DotPlotWidget::setWordSize(int size)
{
    m_wordSize = size;
    adjustScrollbars();
}

void DotPlotWidget::setScale(int size)
{
    m_scale = size;
    adjustScrollbars();
}

void DotPlotWidget::adjustScrollbars()
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
