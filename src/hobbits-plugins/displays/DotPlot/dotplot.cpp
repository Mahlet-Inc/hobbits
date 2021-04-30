#include "dotplot.h"
#include "dotplotform.h"
#include <QPainter>
#include "displayhelper.h"
#include "displayresult.h"

DotPlot::DotPlot() :
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
                [](const QJsonObject &parameters) {
                    int wordSize = parameters.value("word_size").toInt();
                    return QString("%1-bit Digraph Plot").arg(wordSize);
                },
                [](QSharedPointer<ParameterDelegate> delegate, QSize size) {
                        Q_UNUSED(size)
                        return new DotPlotForm(delegate);
                });
}

DisplayInterface* DotPlot::createDefaultDisplay()
{
    return new DotPlot();
}

QString DotPlot::name()
{
    return "Dot Plot";
}

QString DotPlot::description()
{
    return "Displays n-bit word repetition on a Cartesian plane (e.g. if word 2 equals word 7, (2, 7) is marked)";
}

QStringList DotPlot::tags()
{
    return {"Generic"};
}

QSharedPointer<DisplayRenderConfig> DotPlot::renderConfig()
{
    return m_renderConfig;
}

void DotPlot::setDisplayHandle(QSharedPointer<DisplayHandle> displayHandle)
{
    m_handle = displayHandle;
}

QSharedPointer<ParameterDelegate> DotPlot::parameterDelegate()
{
    return m_delegate;
}

QSharedPointer<DisplayResult> DotPlot::renderDisplay(QSize viewportSize, const QJsonObject &parameters, QSharedPointer<PluginActionProgress> progress)
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
    qint64 startBit = (m_handle->currentContainer()->frameAt(frameOffset).start() / wordSize) * wordSize;

    QRgb background = qRgb(0, 0, 0);
    QRgb foreground = qRgb(200, 230, 255);
    QImage dotPlot(windowSize, windowSize, QImage::Format_ARGB32);
    dotPlot.fill(background);

    QVector<quint64> words(windowSize);
    for (int word = 0; word < windowSize; word++) {
        qint64 start = startBit + word * wordSize;
        qint64 end = start + wordSize;
        if (bits->sizeInBits() < end) {
            words.resize(word);
            break;
        }
        words[word] = bits->parseUIntValue(start, wordSize);
    }
    for (int x = 0; x < words.size(); x++) {
        for (int y = 0; y < words.size(); y++) {
            if (words.at(x) == words.at(y)) {
                dotPlot.setPixel(x, y, foreground);
            }
        }
    }

    QImage destImage(viewportSize, QImage::Format_ARGB32);
    destImage.fill(Qt::transparent);
    QPainter painter(&destImage);
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.scale(scale, scale);
    painter.drawImage(0, 0, dotPlot);

    qint64 lastBit = startBit + (windowSize * wordSize);
    lastBit = qMin(m_handle->currentContainer()->bits()->sizeInBits() - 1, lastBit);
    m_handle->setRenderedRange(this, Range(startBit, lastBit));

    return DisplayResult::result(destImage, parameters);
}

QSharedPointer<DisplayResult> DotPlot::renderOverlay(QSize viewportSize, const QJsonObject &parameters)
{
    Q_UNUSED(viewportSize)
    Q_UNUSED(parameters)
    return DisplayResult::nullResult();
}
