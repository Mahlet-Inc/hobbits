#ifndef PLUGINHELPER_H
#define PLUGINHELPER_H

#include "analyzerresult.h"
#include "operatorresult.h"
#include "actionprogress.h"

namespace PluginHelper
{
    void recordProgress(QSharedPointer<ActionProgress> tracker, qint64 completed, qint64 required);
    void recordProgress(QSharedPointer<ActionProgress> tracker, int completed, int required);
    void recordProgress(QSharedPointer<ActionProgress> tracker, double completed, double required);
    QSharedPointer<const OperatorResult> operatorErrorResult(QString error);
    QSharedPointer<const AnalyzerResult> analyzerErrorResult(QString error);
};

#endif // PLUGINHELPER_H
