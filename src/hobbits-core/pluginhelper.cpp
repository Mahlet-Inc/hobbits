#include "pluginhelper.h"


void PluginHelper::recordProgress(QSharedPointer<ActionProgress> tracker, qint64 completed, qint64 required)
{
    int percent = int(double(completed) / double(required) * 100.0);
    tracker->setProgressPercent(percent);
}

void PluginHelper::recordProgress(QSharedPointer<ActionProgress> tracker, int completed, int required)
{
    int percent = int(double(completed) / double(required) * 100.0);
    tracker->setProgressPercent(percent);
}

void PluginHelper::recordProgress(QSharedPointer<ActionProgress> tracker, double completed, double required)
{
    int percent = int(completed / required * 100.0);
    tracker->setProgressPercent(percent);
}

QSharedPointer<const OperatorResult> PluginHelper::operatorErrorResult(QString error)
{
    return QSharedPointer<const OperatorResult>(
            (new OperatorResult())->setPluginState(
                    QJsonObject(
                            {QPair<QString, QJsonValue>(
                                    "error",
                                    QJsonValue(error))}))
            );
}

QSharedPointer<const AnalyzerResult> PluginHelper::analyzerErrorResult(QString error)
{
    return QSharedPointer<const AnalyzerResult>(
            (new AnalyzerResult())->setPluginState(
                    QJsonObject(
                            {QPair<QString, QJsonValue>(
                                    "error",
                                    QJsonValue(error))}))
            );
}
