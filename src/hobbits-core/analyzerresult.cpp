#include "analyzerresult.h"

AnalyzerResult::AnalyzerResult()
{

}

AnalyzerResult* AnalyzerResult::setBitInfo(QSharedPointer<BitInfo> bitInfo)
{
    m_bitInfo = bitInfo;
    return this;
}

QSharedPointer<BitInfo> AnalyzerResult::bitInfo() const
{
    return m_bitInfo;
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

QSharedPointer<const AnalyzerResult> AnalyzerResult::result(QSharedPointer<BitInfo> bitInfo, QJsonObject pluginState)
{

    return QSharedPointer<const AnalyzerResult>(
                (new AnalyzerResult())->setPluginState(pluginState)->setBitInfo(bitInfo)
            );
}

QSharedPointer<const AnalyzerResult> AnalyzerResult::error(QString error)
{

    return QSharedPointer<const AnalyzerResult>(
            (new AnalyzerResult())->setPluginState(
                    QJsonObject(
                            {QPair<QString, QJsonValue>(
                                    "error",
                                    QJsonValue(error))}))
            );
}
