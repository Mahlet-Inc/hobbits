#include "symbolraster.h"
#include "symbolrastercontrols.h"
#include "displayhelper.h"
#include "displayresult.h"
#include <QJsonArray>
#include <QPainter>

SymbolRaster::SymbolRaster() :
    m_renderConfig(new DisplayRenderConfig()),
    m_symbolLength(0)
{
    m_renderConfig->setFullRedrawTriggers(DisplayRenderConfig::NewBitOffset | DisplayRenderConfig::NewFrameOffset);
    m_renderConfig->setOverlayRedrawTriggers(DisplayRenderConfig::NewBitHover);

    QList<ParameterDelegate::ParameterInfo> infos = {
        {"scale", ParameterDelegate::ParameterType::Integer},
        {"show_headers", ParameterDelegate::ParameterType::Boolean},
        {"color_map", ParameterDelegate::ParameterType::Array}
    };

    m_delegate = ParameterDelegate::create(
                    infos,
                    [](const QJsonObject &parameters) {
                        int scale = parameters.value("scale").toInt();
                        if (parameters.value("show_headers").toBool()) {
                            return QString("Symbol Raster %1x with headers").arg(scale);
                        }
                        else {
                            return QString("Byte Raster %1x without headers").arg(scale);
                        }
                    },
                    [](QSharedPointer<ParameterDelegate> delegate, QSize size) {
                        Q_UNUSED(size)
                        return new SymbolRasterControls(delegate);
                    });
}

DisplayInterface* SymbolRaster::createDefaultDisplay()
{
    return new SymbolRaster();
}

QString SymbolRaster::name()
{
    return "Symbol Raster";
}

QString SymbolRaster::description()
{
    return "Displays each n-bit symbol in the data as a uniquely colored pixel";
}

QStringList SymbolRaster::tags()
{
    return {"Generic"};
}

QSharedPointer<DisplayRenderConfig> SymbolRaster::renderConfig()
{
    return m_renderConfig;
}

void SymbolRaster::setDisplayHandle(QSharedPointer<DisplayHandle> displayHandle)
{
    m_handle = displayHandle;
    DisplayHelper::connectHoverUpdates(this, this, m_handle, [this](QPoint& offset, QSize &symbolSize, int &grouping, int &bitsPerSymbol) {
        if (!m_delegate->validate(m_lastParams).isEmpty() || m_symbolLength < 1) {
            return false;
        }
        int scale = m_lastParams.value("scale").toInt();
        offset = headerOffset(m_lastParams);
        symbolSize = QSize(scale, scale);
        grouping = 1;
        bitsPerSymbol = m_symbolLength;
        return true;
    });
}

QSharedPointer<ParameterDelegate> SymbolRaster::parameterDelegate()
{
    return m_delegate;
}

QSharedPointer<DisplayResult> SymbolRaster::renderDisplay(QSize viewportSize, const QJsonObject &parameters, QSharedPointer<PluginActionProgress> progress)
{
    Q_UNUSED(progress)
    m_lastParams = parameters;

    QStringList invalidations = m_delegate->validate(parameters);
    if (!invalidations.isEmpty()) {
        m_handle->setRenderedRange(this, Range());
        return DisplayResult::error(QString("Invalid parameters passed to %1:\n%2").arg(name()).arg(invalidations.join("\n")));
    }
    if (m_handle.isNull() || m_handle->currentContainer().isNull()) {
        m_handle->setRenderedRange(this, Range());
        return DisplayResult::nullResult();
    }

    QJsonArray colorMapValues = parameters.value("color_map").toArray();
    if (colorMapValues.size() < 1) {
        return DisplayResult::error("Empty color map parameter");
    }
    m_symbolLength = colorMapValues.at(0).toObject().value("value").toString().length();
    if (m_symbolLength < 1) {
        return DisplayResult::error("Empty color map symbol");
    }

    int scale = parameters.value("scale").toInt();
    if (scale <= 0) {
        return DisplayResult::error(QString("Invalid scale value: %1").arg(scale));
    }

    QPoint offset = headerOffset(parameters);
    QSize rasterSize(viewportSize.width() - offset.x(), viewportSize.height() - offset.y());
    QSize sourceSize(qMax(1, rasterSize.width() / scale), qMax(1, rasterSize.height() / scale));

    QImage raster = getSymbolMapImage(sourceSize, parameters);

    QImage destImage(viewportSize, QImage::Format_ARGB32);
    destImage.fill(Qt::transparent);
    QPainter painter(&destImage);
    painter.translate(offset);
    painter.scale(scale, scale);
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.drawImage(0, 0, raster);

    painter.resetTransform();
    painter.translate(offset);
    QSize highlightSize(sourceSize.width() * 8, sourceSize.height());
    DisplayHelper::drawHighlights(
                m_handle,
                &painter,
                QSizeF(double(scale) / double(m_symbolLength), scale),
                highlightSize,
                (m_handle->bitOffset() / m_symbolLength) * m_symbolLength,
                m_handle->frameOffset(),
                1);

    DisplayHelper::setRenderRange(this, m_handle, sourceSize.height());

    return DisplayResult::result(destImage, parameters);
}

QSharedPointer<DisplayResult> SymbolRaster::renderOverlay(QSize viewportSize, const QJsonObject &parameters)
{
    m_lastParams = parameters;
    QStringList invalidations = m_delegate->validate(parameters);
    if (!invalidations.isEmpty()) {
        return DisplayResult::error(QString("Invalid parameters passed to %1:\n%2").arg(name()).arg(invalidations.join("\n")));
    }

    QJsonArray colorMapValues = parameters.value("color_map").toArray();
    if (colorMapValues.size() < 1) {
        return DisplayResult::error("Empty color map parameter");
    }
    m_symbolLength = colorMapValues.at(0).toObject().value("value").toString().length();
    if (m_symbolLength < 1) {
        return DisplayResult::error("Empty color map symbol");
    }

    int scale = parameters.value("scale").toInt();

    auto overlay = DisplayHelper::drawHeadersFull(
                viewportSize,
                headerOffset(parameters),
                m_handle,
                QSizeF(double(scale) / double(m_symbolLength), scale));

    return DisplayResult::result(overlay, parameters);
}

QPoint SymbolRaster::headerOffset(const QJsonObject &parameters)
{
    if (!parameters.value("show_headers").toBool() || m_handle->currentContainer().isNull()) {
        return QPoint(0, 0);
    }

    auto font = DisplayHelper::monoFont(10);
    auto container = m_handle->currentContainer();
    auto margin = DisplayHelper::textSize(font, "0").width() * 2;
    return  QPoint(
                DisplayHelper::textSize(font, container->frameCount()).width() + margin,
                DisplayHelper::textSize(font, container->maxFrameWidth()).width() + margin);
}

QImage SymbolRaster::getSymbolMapImage(const QSize &size, const QJsonObject &parameters)
{
    if (m_symbolLength < 1) {
        return QImage();
    }

    QJsonArray colorMapValues = parameters.value("color_map").toArray();
    QMap<quint64, QColor> colorMapping;
    for (auto mappingValue: colorMapValues) {
        QJsonObject mapping = mappingValue.toObject();
        bool ok = false;
        quint64 symbolValue = mapping.value("value").toString().toULongLong(&ok, 2);
        if (ok) {
            colorMapping.insert(symbolValue, QColor(mapping.value("color").toString()));
        }
    }

    QImage raster(size, QImage::Format_ARGB32);
    raster.fill(Qt::transparent);


    qint64 frameOffset = m_handle->frameOffset();
    qint64 symbolOffset = m_handle->bitOffset() / m_symbolLength;
    qint64 bitOffset = symbolOffset * m_symbolLength;

    if (frameOffset < 0) {
        return raster;
    }

    auto container = m_handle->currentContainer();

    for (int i = 0; i < size.height(); i++) {
        if (i + frameOffset >= container->frameCount()) {
            break;
        }
        Frame frame = container->frameAt(i + frameOffset);

        for (int ii = 0; ii < size.width() * m_symbolLength; ii += m_symbolLength) {
            if (ii + bitOffset + m_symbolLength > frame.size()) {
                break;
            }

            quint64 symbolVal = 0;
            for (int bit = m_symbolLength - 1; bit >= 0; bit--) {
                if (frame.at(ii + bit + bitOffset)) {
                    symbolVal |= 0x01 << (m_symbolLength - bit - 1);
                }
            }

            if (colorMapping.contains(symbolVal)) {
                raster.setPixel(ii / m_symbolLength, i, colorMapping.value(symbolVal).rgba());
            }

        }
    }
    return raster;
}
