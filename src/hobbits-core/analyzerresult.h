#ifndef ANALYZERRESULT_H
#define ANALYZERRESULT_H

#include "bitinfo.h"
#include "range.h"
#include <QJsonObject>
#include <QMap>

#include "hobbits-core_global.h"

class HOBBITSCORESHARED_EXPORT AnalyzerResult
{
public:
    AnalyzerResult();

    AnalyzerResult* setBitInfo(QSharedPointer<BitInfo> bitInfo);
    QSharedPointer<BitInfo> bitInfo() const;

    AnalyzerResult* setPluginState(QJsonObject pluginState);
    const QJsonObject getPluginState() const;

private:
    QSharedPointer<BitInfo> m_bitInfo;
    QJsonObject m_pluginState;
};

#endif // ANALYZERRESULT_H
