#include "analyzerresult.h"

AnalyzerResult::AnalyzerResult()
{

}

AnalyzerResult* AnalyzerResult::addRanges(QString key, QList<Range> ranges)
{
    if (m_ranges.contains(key)) {
        m_ranges.insert(key, m_ranges.value(key) + ranges);
    }
    else {
        m_ranges.insert(key, ranges);
    }
    return this;
}

const QMap<QString, QList<Range>> AnalyzerResult::getRanges() const
{
    return m_ranges;
}

AnalyzerResult* AnalyzerResult::addMetadata(QString key, QString value)
{
    m_metadata.insert(key, (QStringList)(value));
    return this;
}

AnalyzerResult* AnalyzerResult::addMetadata(QString key, QStringList value)
{
    m_metadata.insert(key, value);
    return this;
}

const QMap<QString, QStringList> AnalyzerResult::getMetadata() const
{
    return m_metadata;
}

AnalyzerResult* AnalyzerResult::setPluginState(QJsonObject pluginState)
{
    m_pluginState = pluginState;
    return this;
}

const QJsonObject AnalyzerResult::getPluginState() const
{
    return m_pluginState;
}
