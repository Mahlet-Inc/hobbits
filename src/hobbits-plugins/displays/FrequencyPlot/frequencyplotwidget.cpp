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
    auto frames = m_displayHandle->getContainer()->info()->frames();
    auto frameOffset = m_displayHandle->getFrameOffset();
    if (frames->size() <= frameOffset) {
        return;
    }
    qint64 startBit = (frames->at(frameOffset).start() / m_wordSize) * m_wordSize;

    quint64 barCount = 1ull << m_wordSize;
    int valueIdxShift = 0;
    if (m_wordSize == 64) {
        barCount = 2048;
        valueIdxShift = 53;
    }
    QRgb background = qRgb(0, 0, 0);
    QRgb foreground = qRgb(200, 230, 255);

    while (barCount > 2048) {
        barCount >>= 1;
        valueIdxShift++;
    }

    int plotSize = int(barCount);

    QMap<quint64, int> frequencies;
    for (qint64 word = 0; word < m_windowSize; word++) {
        qint64 start = startBit + word * m_wordSize;
        qint64 end = start + m_wordSize;
        if (bits->sizeInBits() < end) {
            break;
        }
        quint64 wordValue = bits->parseUIntValue(start, int(m_wordSize));
        if (!frequencies.contains(wordValue)) {
            frequencies.insert(wordValue, 0);
        }
        frequencies[wordValue]++;
    }

    QMap<int, quint64> barMax;
    QVector<int> barFrequencies(plotSize);
    int frequencyMax = 1;
    for (quint64 wordValue : frequencies.keys()) {
        quint64 idx = wordValue >> valueIdxShift;
        int frequency = frequencies.value(wordValue);
        if (frequency > barFrequencies.at(int(idx))) {
            barFrequencies[int(idx)] = frequencies.value(wordValue);
            barMax.insert(int(idx), wordValue);
            frequencyMax = qMax(frequencyMax, frequency);
        }
    }


    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.scale(m_scale, m_scale);

    int plotHeight = qMin(plotSize, 512);
    painter.fillRect(0, 0, plotSize, plotHeight, background);

    double ratio = -1.0*double(plotHeight)/double(frequencyMax);
    for (int x = 0; x < barFrequencies.size(); x++) {
        double amount = double(barFrequencies.at(x))*ratio;
        painter.fillRect(QRectF(x, plotHeight, 1, amount), foreground);
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
