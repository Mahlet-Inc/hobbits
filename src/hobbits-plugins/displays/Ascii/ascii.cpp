#include "ascii.h"
#include "asciiform.h"
#include "displayhelper.h"
#include "asciidecode.h"
#include "cp437decode.h"
#include "displayresult.h"

Ascii::Ascii() :
    m_renderConfig(new DisplayRenderConfig())
{
    m_renderConfig->setFullRedrawTriggers(DisplayRenderConfig::NewBitOffset | DisplayRenderConfig::NewFrameOffset);
    m_renderConfig->setOverlayRedrawTriggers(DisplayRenderConfig::NewBitHover);


    QList<ParameterDelegate::ParameterInfo> infos = {
        {"font_size", ParameterDelegate::ParameterType::Integer},
        {"column_grouping", ParameterDelegate::ParameterType::Integer},
        {"show_headers", ParameterDelegate::ParameterType::Boolean},
        {"encoding", ParameterDelegate::ParameterType::String}
    };

    m_delegate = ParameterDelegate::create(
                infos,
                [](const Parameters &parameters) {
                    if (parameters.value("show_headers").toBool()) {
                        return QString("ASCII with headers");
                    }
                    else {
                        return QString("ASCII without headers");
                    }
                },
                [](QSharedPointer<ParameterDelegate> delegate, QSize size) {
                        Q_UNUSED(size)
                        return new AsciiForm(delegate);
});
}

DisplayInterface* Ascii::createDefaultDisplay()
{
    return new Ascii();
}

QString Ascii::name()
{
    return "ASCII";
}

QString Ascii::description()
{
    return "Displays data as ASCII characters";
}

QStringList Ascii::tags()
{
    return {"Generic"};
}

QSharedPointer<DisplayRenderConfig> Ascii::renderConfig()
{
    return m_renderConfig;
}

void Ascii::setDisplayHandle(QSharedPointer<DisplayHandle> displayHandle)
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
        bitsPerSymbol = 8;
        return true;
    });
}

QSharedPointer<ParameterDelegate> Ascii::parameterDelegate()
{
    return m_delegate;
}

QSharedPointer<DisplayResult> Ascii::renderDisplay(QSize viewportSize, const Parameters &parameters, QSharedPointer<PluginActionProgress> progress)
{
    Q_UNUSED(progress)
    m_lastParams = parameters;
    QStringList invalidations = m_delegate->validate(parameters);
    if (!invalidations.isEmpty()) {
        return DisplayResult::error(QString("Invalid parameters passed to %1:\n%2").arg(name()).arg(invalidations.join("\n")));
    }

    QString encoding = parameters.value("encoding").toString();
    std::function<QString(const Frame&, qint64 &)> decodeFunction;
    if (encoding == "CP437") {
        decodeFunction = &decodeCp437Bits;
    }
    else {
        decodeFunction = &decodeAsciiBits;
    }

    auto image = DisplayHelper::drawTextRasterFull(viewportSize, headerOffset(parameters), this, m_handle, parameters, 8, decodeFunction);
    return DisplayResult::result(image, parameters);
}

QSharedPointer<DisplayResult> Ascii::renderOverlay(QSize viewportSize, const Parameters &parameters)
{
    if (!m_delegate->validate(m_lastParams).isEmpty()) {
        return DisplayResult::nullResult();
    }
    QSize fontSize = DisplayHelper::textSize(DisplayHelper::monoFont(m_lastParams.value("font_size").toInt()), "0");
    int columnGrouping = m_lastParams.value("column_grouping").toInt();

    auto image = DisplayHelper::drawHeadersFull(
                viewportSize,
                headerOffset(parameters),
                m_handle,
                QSizeF(double(fontSize.width()) / 8.0, DisplayHelper::textRowHeight(fontSize.height())),
                columnGrouping,
                columnGrouping > 1 ? 1 : 0);

    return DisplayResult::result(image, parameters);
}

QPoint Ascii::headerOffset(const Parameters &parameters)
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
