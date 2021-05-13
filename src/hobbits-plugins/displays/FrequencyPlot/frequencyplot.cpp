#include "frequencyplot.h"
#include "frequencyplotform.h"
#include <QPainter>
#include "displayhelper.h"
#include "displayresult.h"

FrequencyPlot::FrequencyPlot() :
    m_renderConfig(new DisplayRenderConfig())
{
    m_renderConfig->setFullRedrawTriggers(DisplayRenderConfig::NewBitOffset | DisplayRenderConfig::NewFrameOffset);
    m_renderConfig->setOverlayRedrawTriggers(DisplayRenderConfig::NewMouseHover);

    QList<ParameterDelegate::ParameterInfo> infos = {
        {"scale", ParameterDelegate::ParameterType::Integer},
        {"word_size", ParameterDelegate::ParameterType::Integer},
        {"window_size", ParameterDelegate::ParameterType::Integer}
    };

    m_delegate = ParameterDelegate::create(
                infos,
                [](const Parameters &parameters) {
                    int wordSize = parameters.value("word_size").toInt();
                    return QString("%1-bit Frequency Plot").arg(wordSize);
    },
    [](QSharedPointer<ParameterDelegate> delegate, QSize size) {
            Q_UNUSED(size)
            return new FrequencyPlotForm(delegate);
});
}

DisplayInterface* FrequencyPlot::createDefaultDisplay()
{
    return new FrequencyPlot();
}

QString FrequencyPlot::name()
{
    return "Frequency Plot";
}

QString FrequencyPlot::description()
{
    return "Displays a histogram where each bar shows the frequency of an n-bit word across the given sample window";
}

QStringList FrequencyPlot::tags()
{
    return {"Generic"};
}

QSharedPointer<DisplayRenderConfig> FrequencyPlot::renderConfig()
{
    return m_renderConfig;
}

void FrequencyPlot::setDisplayHandle(QSharedPointer<DisplayHandle> displayHandle)
{
    m_handle = displayHandle;
}

QSharedPointer<ParameterDelegate> FrequencyPlot::parameterDelegate()
{
    return m_delegate;
}

QSharedPointer<DisplayResult> FrequencyPlot::renderDisplay(QSize viewportSize, const Parameters &parameters, QSharedPointer<PluginActionProgress> progress)
{
    Q_UNUSED(progress)
    m_barMax.clear();
    m_barFrequencies.clear();

    QStringList invalidations = m_delegate->validate(parameters);
    if (!invalidations.isEmpty()) {
        m_handle->setRenderedRange(this, Range());
        return DisplayResult::error(QString("Invalid parameters passed to %1:\n%2").arg(name()).arg(invalidations.join("\n")));
    }
    if (m_handle.isNull() || m_handle->currentContainer().isNull()) {
        m_handle->setRenderedRange(this, Range());
        return DisplayResult::nullResult();
    }

    int wordSize = parameters.value("word_size").toInt(8);
    int windowSize = parameters.value("window_size").toInt(10000);
    int scale = parameters.value("scale").toInt(2);
    if (scale <= 0) {
        return DisplayResult::error(QString("Invalid scale value: %1").arg(scale));
    }

    auto bits = m_handle->currentContainer()->bits();
    auto frameOffset = m_handle->frameOffset();
    if (m_handle->currentContainer()->frameCount() <= frameOffset) {
        m_handle->setRenderedRange(this, Range());
        return DisplayResult::error("Invalid frame offset");
    }
    qint64 startBit = (m_handle->currentContainer()->frameAt(frameOffset).start() / wordSize) * wordSize;

    quint64 barCount = 1ull << wordSize;
    int valueIdxShift = 0;
    if (wordSize == 64) {
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
    for (qint64 word = 0; word < windowSize; word++) {
        qint64 start = startBit + word * wordSize;
        qint64 end = start + wordSize;
        if (bits->sizeInBits() < end) {
            break;
        }
        quint64 wordValue = bits->parseUIntValue(start, int(wordSize));
        if (!frequencies.contains(wordValue)) {
            frequencies.insert(wordValue, 0);
        }
        frequencies[wordValue]++;
    }

    m_barFrequencies.resize(plotSize);
    int frequencyMax = 1;
    for (quint64 wordValue : frequencies.keys()) {
        quint64 idx = wordValue >> valueIdxShift;
        int frequency = frequencies.value(wordValue);
        if (frequency > m_barFrequencies.at(int(idx))) {
            m_barFrequencies[int(idx)] = frequencies.value(wordValue);
            m_barMax.insert(int(idx), wordValue);
            frequencyMax = qMax(frequencyMax, frequency);
        }
    }


    QImage destImage(viewportSize, QImage::Format_ARGB32);
    destImage.fill(Qt::transparent);
    QPainter painter(&destImage);

    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.scale(scale, scale);

    int plotHeight = qMin(plotSize, 512);
    painter.fillRect(0, 0, plotSize, plotHeight, background);

    double ratio = -1.0*double(plotHeight)/double(frequencyMax);
    for (int x = 0; x < m_barFrequencies.size(); x++) {
        double amount = double(m_barFrequencies.at(x))*ratio;
        painter.fillRect(QRectF(x, plotHeight, 1, amount), foreground);
    }

    qint64 lastBit = startBit + (windowSize * wordSize);
    lastBit = qMin(m_handle->currentContainer()->bits()->sizeInBits() - 1, lastBit);
    m_handle->setRenderedRange(this, Range(startBit, lastBit));

    return DisplayResult::result(destImage, parameters);
}

QSharedPointer<DisplayResult> FrequencyPlot::renderOverlay(QSize viewportSize, const Parameters &parameters)
{
    QStringList invalidations = m_delegate->validate(parameters);
    if (!invalidations.isEmpty()) {
        return DisplayResult::error(QString("Invalid parameters passed to %1:\n%2").arg(name()).arg(invalidations.join("\n")));
    }
    QPoint hover = m_handle->mouseHover(this);
    if (m_handle.isNull()
            || m_handle->currentContainer().isNull()
            || !m_delegate->validate(parameters).isEmpty()
            || hover.isNull()
            || hover.x() < 0
            || hover.y() < 0) {
        return DisplayResult::nullResult();
    }

    int scale = parameters.value("scale").toInt(2);

    QImage destImage(viewportSize, QImage::Format_ARGB32);
    destImage.fill(Qt::transparent);
    QPainter painter(&destImage);

    int barIdx = hover.x() / scale;

    if (!m_barMax.contains(barIdx)
            || barIdx >= m_barFrequencies.size()) {
        return DisplayResult::nullResult();
    }

    quint64 value = m_barMax.value(barIdx);
    int frequency = m_barFrequencies.at(barIdx);

    QString decimal = QString("%1").arg(value, 0, 10);
    QString hex = QString("0x%1").arg(value, 0, 16);
    QString freq = QString("Count: %1").arg(frequency);

    DisplayHelper::drawHoverBox(&painter, QRect({0, 0}, viewportSize), hover, {decimal, hex, freq});

    return DisplayResult::result(destImage, parameters);
}
