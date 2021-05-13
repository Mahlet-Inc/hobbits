#include "hex.h"
#include "hexform.h"
#include "displayresult.h"
#include "displayhelper.h"

Hex::Hex() :
    m_renderConfig(new DisplayRenderConfig())
{
    m_renderConfig->setFullRedrawTriggers(DisplayRenderConfig::NewBitOffset | DisplayRenderConfig::NewFrameOffset);
    m_renderConfig->setOverlayRedrawTriggers(DisplayRenderConfig::NewBitHover);

    QList<ParameterDelegate::ParameterInfo> infos = {
        {"font_size", ParameterDelegate::ParameterType::Integer},
        {"column_grouping", ParameterDelegate::ParameterType::Integer},
        {"show_headers", ParameterDelegate::ParameterType::Boolean}
    };

    m_delegate = ParameterDelegate::create(
                infos,
                [](const Parameters &parameters) {
                    if (parameters.value("show_headers").toBool()) {
                        return QString("Hex with headers");
                    }
                    else {
                        return QString("Hex without headers");
                    }
                },
                [](QSharedPointer<ParameterDelegate> delegate, QSize size) {
                        Q_UNUSED(size)
                        return new HexForm(delegate);
                });
}

DisplayInterface* Hex::createDefaultDisplay()
{
    return new Hex();
}

QString Hex::name()
{
    return "Hex";
}

QString Hex::description()
{
    return "Displays data as a hexadecimal string";
}

QStringList Hex::tags()
{
    return {"Generic"};
}

QSharedPointer<DisplayRenderConfig> Hex::renderConfig()
{
    return m_renderConfig;
}

void Hex::setDisplayHandle(QSharedPointer<DisplayHandle> displayHandle)
{
    m_handle = displayHandle;
    DisplayHelper::connectHoverUpdates(this, this, m_handle, [this](QPoint& offset, QSize &symbolSize, int &grouping, int &bitsPerSymbol) {
        if (!m_delegate->validate(m_lastParams).isEmpty()) {
            return false;
        }
        offset = headerOffset(m_lastParams);
        QSize fontSize = DisplayHelper::textSize(DisplayHelper::monoFont(m_lastParams.value("font_size").toInt()), "0");
        symbolSize.setWidth(fontSize.width());
        symbolSize.setHeight(DisplayHelper::textRowHeight(fontSize.height()));
        grouping = m_lastParams.value("column_grouping").toInt();
        bitsPerSymbol = 4;
        return true;
    });
}

QSharedPointer<ParameterDelegate> Hex::parameterDelegate()
{
    return m_delegate;
}

QSharedPointer<DisplayResult> Hex::renderDisplay(QSize viewportSize, const Parameters &parameters, QSharedPointer<PluginActionProgress> progress)
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

    auto image = DisplayHelper::drawTextRasterFull(viewportSize, headerOffset(parameters), this, m_handle, parameters, 4,
                                             [](const Frame& f, qint64 &bitOffset) {
        QString hexString;
        if (f.size() > bitOffset + 3) {
            int nibble = 0;
            for (int bit = 0; bit < 4; bit++) {
                nibble <<= 1;
                if (f.at(bitOffset + bit)) {
                    nibble |= 0x01;
                }
            }
            hexString.setNum(nibble, 16);
        }
        else {
            for (qint64 i = bitOffset; i < f.size(); i++) {
                hexString += f.at(i) ? "." : "o";
            }
        }
        bitOffset += 4;
        return hexString;
    });

    return DisplayResult::result(image, parameters);
}

QSharedPointer<DisplayResult> Hex::renderOverlay(QSize viewportSize, const Parameters &parameters)
{
    QStringList invalidations = m_delegate->validate(parameters);
    if (!invalidations.isEmpty()) {
        return DisplayResult::error(QString("Invalid parameters passed to %1:\n%2").arg(name()).arg(invalidations.join("\n")));
    }
    if (m_handle.isNull() || m_handle->currentContainer().isNull()) {
        return DisplayResult::nullResult();
    }

    QSize fontSize = DisplayHelper::textSize(DisplayHelper::monoFont(m_lastParams.value("font_size").toInt()), "0");
    int columnGrouping = m_lastParams.value("column_grouping").toInt();

    auto overlay = DisplayHelper::drawHeadersFull(
                viewportSize,
                headerOffset(parameters),
                m_handle,
                QSizeF(double(fontSize.width()) / 4.0, DisplayHelper::textRowHeight(fontSize.height())),
                columnGrouping,
                columnGrouping > 1 ? 1 : 0);

    return DisplayResult::result(overlay, parameters);
}


QPoint Hex::headerOffset(const Parameters &parameters)
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
