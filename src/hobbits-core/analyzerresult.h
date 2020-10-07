#ifndef ANALYZERRESULT_H
#define ANALYZERRESULT_H

#include "bitinfo.h"
#include "range.h"
#include <QJsonObject>
#include <QMap>
#include "hobbits-core_global.h"

/**
  * @brief The AnalyzerResult class contains the result of an AnalyzerInterface run
  *
  * \see AnalyzerInterface
*/
class HOBBITSCORESHARED_EXPORT AnalyzerResult
{
public:
    AnalyzerResult();

    AnalyzerResult* setBitInfo(QSharedPointer<BitInfo> bitInfo);
    QSharedPointer<BitInfo> bitInfo() const;

    AnalyzerResult* setParameters(QJsonObject parameters);
    const QJsonObject parameters() const;

    bool hasEmptyParameters() const;
    QString errorString() const;

    static QSharedPointer<const AnalyzerResult> result(QSharedPointer<BitInfo> bitInfo, QJsonObject parameters);
    static QSharedPointer<const AnalyzerResult> error(QString errorString);

private:
    QSharedPointer<BitInfo> m_bitInfo;
    QJsonObject m_parameters;
    QString m_errorString;
};

#endif // ANALYZERRESULT_H
