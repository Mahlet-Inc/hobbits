#include "find.h"
#include "findform.h"
#include "settingsmanager.h"

Find::Find()
{
    QList<ParameterDelegate::ParameterInfo> infos = {
        {"search_string", ParameterDelegate::ParameterType::String}
    };

    m_delegate = ParameterDelegate::create(
                    infos,
                    [](const Parameters &parameters) {
        return QString("Find %1").arg(parameters.value("search_string").toString());
    },
    [](QSharedPointer<ParameterDelegate> delegate, QSize size) {
                    Q_UNUSED(size)
                    return new FindForm(delegate);
                });
}

AnalyzerInterface* Find::createDefaultAnalyzer()
{
    return new Find();
}

QString Find::name()
{
    return "Find";
}

QString Find::description()
{
    return "Find ";
}

QStringList Find::tags()
{
    return {"Generic"};
}

QSharedPointer<ParameterDelegate>  Find::parameterDelegate()
{
    return m_delegate;
}

QSharedPointer<const AnalyzerResult> Find::analyzeBits(
        QSharedPointer<const BitContainer> container,
        const Parameters &parameters,
        QSharedPointer<PluginActionProgress> progress)
{
    QStringList invalidations = m_delegate->validate(parameters);
    if (!invalidations.isEmpty()) {
        return AnalyzerResult::error(QString("Invalid parameters passed to %1:\n%2").arg(name()).arg(invalidations.join("\n")));
    }

    auto findBits = BitArray::fromString(parameters.value("search_string").toString());
    auto bits = container->bits();

    if (findBits->sizeInBits() < 1) {
        return AnalyzerResult::error(QString("Find bits string is empty"));
    }

    QColor findColor = SettingsManager::getPluginSetting(FIND_COLOR_KEY).value<QColor>();
    QList<RangeHighlight> highlights;

    qint64 prev = 0;
    for (qint64 start = 0; start < bits->sizeInBits(); start++) {
        if (findBits->sizeInBits() > bits->sizeInBits() - start) {
            break;
        }
        bool match = true;
        for (int i = 0; i < findBits->sizeInBits() && start + i < bits->sizeInBits(); i++) {
            if (bits->at(start + i) != findBits->at(i)) {
                match = false;
                break;
            }
        }
        if (match) {
            qint64 end = start + findBits->sizeInBits() - 1;
            QString label = QString("bit: %1 , delta: %2").arg(start).arg(start - prev);
            highlights.append(RangeHighlight(FOUND_HIGHLIGHT, label, Range(start, end), findColor.rgba()));
            prev = start;
            start = end;
            end = end + findBits->sizeInBits() - 1;
        }

        progress->setProgress(start, bits->sizeInBits());
        if (progress->isCancelled()) {
            return AnalyzerResult::error("Processing cancelled");
        }
    }

    QSharedPointer<BitInfo> bitInfo = BitInfo::copyFromContainer(container);
    bitInfo->clearHighlightCategory(FOUND_HIGHLIGHT);
    bitInfo->addHighlights(highlights);

    QString resultHead = "";
    if (findBits->sizeInBits() % 8 == 0) {
        resultHead += "0x" + findBits->readBytes(0, findBits->sizeInBytes()).toHex();
    }
    else {
        for (int i = 0; i < findBits->sizeInBits(); i++) {
            if (findBits->at(i)) {
                resultHead += "1";
            }
            else {
                resultHead += "0";
            }
        }
    }
    bitInfo->setMetadata(FOUND_RESULT_LABEL, resultHead);

    return AnalyzerResult::result(bitInfo, parameters);
}
