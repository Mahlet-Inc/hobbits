#include "binary.h"
#include "binarycontrols.h"
#include "displayhelper.h"
#include "displayresult.h"
#include <QPainter>
#include <QtMath>

Binary::Binary() :
    m_renderConfig(new DisplayRenderConfig())
{
    m_renderConfig->setFullRedrawTriggers(DisplayRenderConfig::NewBitOffset | DisplayRenderConfig::NewFrameOffset);
    m_renderConfig->setOverlayRedrawTriggers(DisplayRenderConfig::NewBitHover);

    QList<ParameterDelegate::ParameterInfo> infos = {
        {"font_size", QJsonValue::Double},
        {"column_grouping", QJsonValue::Double},
        {"show_headers", QJsonValue::Bool}
    };

    m_delegate = ParameterDelegate::create(
                    infos,
                    [](const QJsonObject &parameters) {
                        if (parameters.value("show_headers").toBool()) {
                            return QString("Binary with headers");
                        }
                        else {
                            return QString("Binary without headers");
                        }
                    },
                    [](QSharedPointer<ParameterDelegate> delegate, QSize size) {
                        Q_UNUSED(size)
                        return new BinaryControls(delegate);
                    });
}

DisplayInterface* Binary::createDefaultDisplay()
{
    return new Binary();
}

QString Binary::name()
{
    return "Binary";
}

QString Binary::description()
{
    return "Display data as 1s and 0s";
}

QStringList Binary::tags()
{
    return {"Generic"};
}

QSharedPointer<DisplayRenderConfig> Binary::renderConfig()
{
    return m_renderConfig;
}

void Binary::setDisplayHandle(QSharedPointer<DisplayHandle> displayHandle)
{
    m_handle = displayHandle;
    DisplayHelper::connectHoverUpdates(this, this, m_handle, [this](QPoint& offset, QSize &symbolSize, int &grouping, int &bitsPerSymbol) {
        if (!m_delegate->validate(m_lastParams)) {
            return false;
        }
        offset = headerOffset(m_lastParams);
        QSize fontSize = DisplayHelper::textSize(DisplayHelper::monoFont(m_lastParams.value("font_size").toInt()), "0");
        symbolSize.setWidth(fontSize.width());
        symbolSize.setHeight(DisplayHelper::textRowHeight(fontSize.height()));
        grouping = m_lastParams.value("column_grouping").toInt();
        bitsPerSymbol = 1;
        return true;
    });
}

QSharedPointer<ParameterDelegate> Binary::parameterDelegate()
{
    return m_delegate;
}

QSharedPointer<DisplayResult> Binary::renderDisplay(QSize viewportSize, const QJsonObject &parameters, QSharedPointer<PluginActionProgress> progress)
{
    Q_UNUSED(progress)
    m_lastParams = parameters;
    if (!m_delegate->validate(parameters)) {
        m_handle->setRenderedRange(this, Range());
        return DisplayResult::error("Invalid parameters");
    }

    auto image = DisplayHelper::drawTextRasterFull(viewportSize, headerOffset(parameters), this, m_handle, parameters, 1,
                                             [](const Frame& f, qint64 &bitOffset) {
        if (f.at(bitOffset)) {
            bitOffset++;
            return "1";
        }
        else {
            bitOffset++;
            return "0";
        }
    });

    return DisplayResult::result(image, parameters);
}

QSharedPointer<DisplayResult> Binary::renderOverlay(QSize viewportSize, const QJsonObject &parameters)
{
    if (!m_delegate->validate(m_lastParams)) {
        return DisplayResult::error("Invalid parameters");
    }
    QSize fontSize = DisplayHelper::textSize(DisplayHelper::monoFont(m_lastParams.value("font_size").toInt()), "0");
    int columnGrouping = m_lastParams.value("column_grouping").toInt();

    auto overlay = DisplayHelper::drawHeadersFull(
                viewportSize,
                headerOffset(parameters),
                m_handle,
                QSizeF(fontSize.width(), DisplayHelper::textRowHeight(fontSize.height())),
                columnGrouping,
                columnGrouping > 1 ? 1 : 0);

    return DisplayResult::result(overlay, parameters);
}

QPoint Binary::headerOffset(const QJsonObject &parameters)
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
