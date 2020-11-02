#include "frequencyplotwidget.h"

#include <QPainter>
#include <QImage>
#include "displayhelper.h"
#include <QMouseEvent>

FrequencyPlotWidget::FrequencyPlotWidget(
        QSharedPointer<DisplayHandle> displayHandle,
        DisplayInterface *pluginRef,
        QWidget *parent) :
    DisplayBase(displayHandle, pluginRef, parent),
    m_windowSize(10000),
    m_wordSize(8),
    m_scale(2),
    m_mouseHover(-1, -1)
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
    painter.save();
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.scale(m_scale, m_scale);

    int plotHeight = qMin(plotSize, 512);
    painter.fillRect(0, 0, plotSize, plotHeight, background);

    double ratio = -1.0*double(plotHeight)/double(frequencyMax);
    for (int x = 0; x < barFrequencies.size(); x++) {
        double amount = double(barFrequencies.at(x))*ratio;
        painter.fillRect(QRectF(x, plotHeight, 1, amount), foreground);
    }
    painter.restore();

    if (m_mouseHover.x() >= 0 && m_mouseHover.y() >= 0) {

        int barIdx = m_mouseHover.x() / m_scale;
        int x = m_mouseHover.x();
        int y = m_mouseHover.y();

        if (!barMax.contains(barIdx)) {
            return;
        }
        if (barIdx >= barFrequencies.size()) {
            return;
        }

        quint64 value = barMax.value(barIdx);
        int frequency = barFrequencies.at(barIdx);

        QString decimal = QString("%1").arg(value, 0, 10);
        QString hex = QString("0x%1").arg(value, 0, 16);
        QString freq = QString("Count: %1").arg(frequency);

        int pad = 4;
        int textHeight = 10 + 3;
        int boxWidth = 10 * qMax(decimal.size(), freq.size()) + pad*2;
        int boxHeight = textHeight * 3 + pad*2;


        QRect box;
        if (x > (plotSize * m_scale) / 2) {
            box.setX(x - pad - boxWidth);
        }
        else {
            box.setX(x + pad);
        }
        if (y > (plotHeight * m_scale) / 2) {
            box.setY(y - pad - boxHeight);
        }
        else {
            box.setY(y + pad);
        }
        box.setWidth(boxWidth);
        box.setHeight(boxHeight);

        painter.save();
        painter.fillRect(box, QColor(0x00, 0x00, 0x00, 0x99));
        QFont font = QFont("Roboto Mono", 10);
        font.setStyleStrategy(QFont::ForceIntegerMetrics);
        painter.setFont(font);
        painter.setPen(QColor(0xff, 0xff, 0xff));
        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
        painter.drawText(box.x() + pad, box.y() + pad, box.width(), textHeight, Qt::AlignLeft, decimal);
        painter.drawText(box.x() + pad, box.y() + pad + textHeight, box.width(), textHeight, Qt::AlignLeft, hex);
        painter.drawText(box.x() + pad, box.y() + pad + (2 * textHeight), box.width(), textHeight, Qt::AlignLeft, freq);

        painter.restore();
    }
}

void FrequencyPlotWidget::leaveEvent(QEvent *event)
{
    m_mouseHover = {-1, -1};
    repaint();
}

void FrequencyPlotWidget::mouseMoveEvent(QMouseEvent *event) {
    m_mouseHover = {-1, -1};

    if (m_displayHandle->getContainer().isNull()) {
        return;
    }

    m_mouseHover.setX(event->x());
    m_mouseHover.setY(event->y());

    repaint();
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
