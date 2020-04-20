#include "frequencyplotwidget.h"

#include <QPainter>
#include <QImage>
#include "displayhelper.h"

FrequencyPlotWidget::FrequencyPlotWidget(
        QSharedPointer<DisplayHandle> displayHandle,
        DisplayInterface *pluginRef,
        QWidget *parent) :
    DisplayBase(displayHandle, pluginRef, parent),
    m_windowSize(10000),
    m_wordSize(8),
    m_scale(2)
{

}

void FrequencyPlotWidget::paintEvent(QPaintEvent*) {
    if (m_displayHandle->getContainer().isNull()) {
        return;
    }

    auto bits = m_displayHandle->getContainer()->bits();
    auto frames = m_displayHandle->getContainer()->bitInfo()->frames();
    auto frameOffset = m_displayHandle->getFrameOffset();
    if (frames.size() <= frameOffset) {
        return;
    }
    qint64 startBit = (frames.at(frameOffset).start() / m_wordSize) * m_wordSize;

    int plotSize = 1 << m_wordSize;
    QRgb background = qRgb(0, 0, 0);
    QRgb foreground = qRgb(200, 230, 255);

    QVector<int> wordFrequency(plotSize);
    int frequencyMax = 1;
    for (qint64 word = 0; word < m_windowSize; word++) {
        qint64 start = startBit + word * m_wordSize;
        qint64 end = start + m_wordSize;
        if (bits->sizeInBits() < end) {
            break;
        }
        int wordValue = int(bits->getWordValue(start, int(m_wordSize)));
        wordFrequency[wordValue]++;
        frequencyMax = qMax(frequencyMax, wordFrequency.at(wordValue));
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.scale(m_scale, m_scale);

    painter.fillRect(0, 0, plotSize, plotSize, background);

    double ratio = -1.0*double(plotSize)/double(frequencyMax);
    for (int x = 0; x < wordFrequency.size(); x++) {
        double amount = double(wordFrequency.at(x))*ratio;
        painter.fillRect(QRectF(x, plotSize, 1, amount), foreground);
    }
}

void FrequencyPlotWidget::mouseMoveEvent(QMouseEvent *event) {
    Q_UNUSED(event)
}

void FrequencyPlotWidget::setWindowSize(int size)
{
    m_windowSize = size;
    adjustScrollbars();
}

void FrequencyPlotWidget::setWordSize(int size)
{
    m_wordSize = size;
    adjustScrollbars();
}

void FrequencyPlotWidget::setScale(int size)
{
    m_scale = size;
    adjustScrollbars();
}

void FrequencyPlotWidget::adjustScrollbars()
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
