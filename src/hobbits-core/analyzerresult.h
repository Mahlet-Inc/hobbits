#ifndef ANALYZERRESULT_H
#define ANALYZERRESULT_H

#include "range.h"
#include <QJsonObject>
#include <QMap>

class AnalyzerResult
{
public:
    AnalyzerResult();

    AnalyzerResult* addRanges(QString key, QList<Range> ranges);
    const QMap<QString, QList<Range>> getRanges() const;

    AnalyzerResult* addMetadata(QString key, QString value);
    AnalyzerResult* addMetadata(QString key, QStringList value);
    const QMap<QString, QStringList> getMetadata() const;

    AnalyzerResult* setPluginState(QJsonObject pluginState);
    const QJsonObject getPluginState() const;

private:
    QMap<QString, QList<Range>> m_ranges;
    QMap<QString, QStringList> m_metadata;

    QJsonObject m_pluginState;
};

#endif // ANALYZERRESULT_H
