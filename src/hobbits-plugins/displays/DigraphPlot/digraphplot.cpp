#include "digraphplot.h"
#include "digraphplotform.h"
#include <QPainter>
#include "displayhelper.h"
#include "displayresult.h"

DigraphPlot::DigraphPlot() :
    m_renderConfig(new DisplayRenderConfig())
{
    m_renderConfig->setFullRedrawTriggers(DisplayRenderConfig::NewBitOffset | DisplayRenderConfig::NewFrameOffset);

    QList<ParameterDelegate::ParameterInfo> infos = {
        {"scale", ParameterDelegate::ParameterType::Integer},
        {"word_size", ParameterDelegate::ParameterType::Integer},
        {"window_size", ParameterDelegate::ParameterType::Integer}
    };

    m_delegate = ParameterDelegate::create(
                infos,
                [](const Parameters &parameters) {
                    int wordSize = parameters.value("word_size").toInt();
                    return QString("%1-bit Digraph Plot").arg(wordSize);
                },
                [](QSharedPointer<ParameterDelegate> delegate, QSize size) {
                        Q_UNUSED(size)
                        return new DigraphPlotForm(delegate);
                });
}

DisplayInterface* DigraphPlot::createDefaultDisplay()
{
    return new DigraphPlot();
}

QString DigraphPlot::name()
{
    return "Digraph Plot";
}

QString DigraphPlot::description()
{
    return "Displays pairs of adjacent n-bit words by their values, x and y, on a Cartesian plane";
}

QStringList DigraphPlot::tags()
{
    return {"Generic"};
}

QSharedPointer<DisplayRenderConfig> DigraphPlot::renderConfig()
{
    return m_renderConfig;
}

void DigraphPlot::setDisplayHandle(QSharedPointer<DisplayHandle> displayHandle)
{
    m_handle = displayHandle;
}

QSharedPointer<ParameterDelegate> DigraphPlot::parameterDelegate()
{
    return m_delegate;
}

QSharedPointer<DisplayResult> DigraphPlot::renderDisplay(QSize viewportSize, const Parameters &parameters, QSharedPointer<PluginActionProgress> progress)
{
    Q_UNUSED(progress)

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
    qint64 startBit = ((m_handle->currentContainer()->frameAt(frameOffset).start() + m_handle->bitOffset()) / wordSize) * wordSize;

    int plotSize = 1 << wordSize;
    QRgb background = qRgb(0, 0, 0);
    QRgb foreground = qRgb(200, 230, 255);
    QImage digraphPlot(plotSize, plotSize, QImage::Format_ARGB32);
    digraphPlot.fill(background);

    int lastWord = 0;
    int nextWord = 0;
    for (qint64 word = 0; word < windowSize; word++) {
        qint64 start = startBit + word * wordSize;
        qint64 end = start + wordSize;
        if (bits->sizeInBits() < end) {
            break;
        }
        lastWord = nextWord;
        nextWord = int(bits->parseUIntValue(start, int(wordSize)));
        if (word == 0) {
            continue;
        }
        digraphPlot.setPixel(lastWord, nextWord, foreground);
    }

    QImage destImage(viewportSize, QImage::Format_ARGB32);
    destImage.fill(Qt::transparent);
    QPainter painter(&destImage);
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.scale(scale, scale);
    painter.drawImage(0, 0, digraphPlot);

    qint64 lastBit = startBit + (windowSize * wordSize);
    lastBit = qMin(m_handle->currentContainer()->bits()->sizeInBits() - 1, lastBit);
    m_handle->setRenderedRange(this, Range(startBit, lastBit));

    return DisplayResult::result(destImage, parameters);
}

QSharedPointer<DisplayResult> DigraphPlot::renderOverlay(QSize viewportSize, const Parameters &parameters)
{
    Q_UNUSED(viewportSize)
    Q_UNUSED(parameters)
    return DisplayResult::nullResult();
}
