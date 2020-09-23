#ifndef ANALYZERINTERFACE_H
#define ANALYZERINTERFACE_H

#include <QSharedPointer>
#include "actionprogress.h"
#include "analyzerresult.h"
#include "bitcontainer.h"
#include "bitcontainerpreview.h"
#include "pluginstatemanager.h"

#include "hobbits-core_global.h"

class HOBBITSCORESHARED_EXPORT AnalyzerInterface
{
public:
    virtual ~AnalyzerInterface() = default;

    virtual AnalyzerInterface* createDefaultAnalyzer() = 0;

    virtual QString name() = 0;
    virtual QString description() = 0;
    virtual QStringList tags() = 0;

    virtual QSharedPointer<PluginStateManager> pluginStateManager() = 0;

    virtual QSharedPointer<const AnalyzerResult> analyzeBits(
            QSharedPointer<const BitContainer> container,
            const QJsonObject &recallablePluginState,
            QSharedPointer<ActionProgress> progressTracker) = 0;

    virtual void previewBits(QSharedPointer<BitContainerPreview> container) = 0;
};

Q_DECLARE_INTERFACE(AnalyzerInterface, "hobbits.AnalyzerInterface")

#endif // ANALYZERINTERFACE_H
