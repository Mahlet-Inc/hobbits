#include "highlight.h"
#include "highlightform.h"
#include "settingsmanager.h"

Highlight::Highlight()
{
    QList<ParameterDelegate::ParameterInfo> infos = {
        {"start", ParameterDelegate::ParameterType::Integer},
        {"length", ParameterDelegate::ParameterType::Integer},
        {"color", ParameterDelegate::ParameterType::Integer, true}
    };

    m_delegate = QSharedPointer<ParameterDelegateUi>(
                new ParameterDelegateUi(
                    infos,
                    [](const QJsonObject &parameters) {
                        return QString("Highlight %2 bits at %1")
                                .arg(parameters.value("length").toInt())
                                .arg(parameters.value("start").toInt());
                    },
                    [](QSharedPointer<ParameterDelegate> delegate, QSize size) {
                        Q_UNUSED(size)
                        return new HighlightForm(delegate);
                    }));
}

AnalyzerInterface* Highlight::createDefaultAnalyzer()
{
    return new Highlight();
}

QString Highlight::name()
{
    return "Highlight";
}

QString Highlight::description()
{
    return "Highlight sections in the data";
}

QStringList Highlight::tags()
{
    return {"Generic"};
}

QSharedPointer<ParameterDelegate>  Highlight::parameterDelegate()
{
    return m_delegate;
}

QSharedPointer<const AnalyzerResult> Highlight::analyzeBits(
        QSharedPointer<const BitContainer> container,
        const QJsonObject &parameters,
        QSharedPointer<PluginActionProgress> progress)
{
    progress->setProgressPercent(5);
    QStringList invalidations = m_delegate->validate(parameters);
    if (!invalidations.isEmpty()) {
        return AnalyzerResult::error(QString("Invalid parameters passed to %1:\n%2").arg(name()).arg(invalidations.join("\n")));
    }

    qint64 start = parameters.value("start").toInt();
    qint64 length = parameters.value("length").toInt();

    quint32 color = SettingsManager::getUiSetting(SettingsManager::HIGHLIGHT_5_COLOR_KEY).value<QColor>().rgba();
    if (parameters.contains("color")) {
        color = parameters.value("color").toVariant().toUInt();
    }

    Range range(start, start + length - 1);
    auto info = BitInfo::create(container->bits()->sizeInBits(), container->info());
    info->addHighlight(RangeHighlight("manual_highlights",
                                      QString("%1 to %2").arg(range.start()).arg(range.end()),
                                      range,
                                      color));

    progress->setProgressPercent(95);
    return AnalyzerResult::result(info, parameters);
}
