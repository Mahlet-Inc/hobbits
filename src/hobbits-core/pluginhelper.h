#ifndef PLUGINHELPER_H
#define PLUGINHELPER_H

#include "analyzerresult.h"
#include "operatorresult.h"
#include "actionprogress.h"

namespace PluginHelper
{
    void HOBBITSCORESHARED_EXPORT recordProgress(QSharedPointer<ActionProgress> tracker, qint64 completed, qint64 required);
    void HOBBITSCORESHARED_EXPORT recordProgress(QSharedPointer<ActionProgress> tracker, int completed, int required);
    void HOBBITSCORESHARED_EXPORT recordProgress(QSharedPointer<ActionProgress> tracker, double completed, double required);
    QSharedPointer<const OperatorResult> HOBBITSCORESHARED_EXPORT operatorErrorResult(QString error);
    QSharedPointer<const AnalyzerResult> HOBBITSCORESHARED_EXPORT analyzerErrorResult(QString error);
};

#endif // PLUGINHELPER_H
