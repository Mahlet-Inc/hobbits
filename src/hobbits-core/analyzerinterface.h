#ifndef ANALYZERINTERFACE_H
#define ANALYZERINTERFACE_H

#include <QSharedPointer>
#include <QStringListModel>

#include "actionprogress.h"
#include "analyzerresult.h"
#include "bitcontainer.h"
#include "bitcontainerpreview.h"
#include "plugincallback.h"

#include "hobbits-core_global.h"

class HOBBITSCORESHARED_EXPORT AnalyzerInterface
{
public:
    virtual ~AnalyzerInterface()
    {
    }

    virtual QString getName() = 0;
    virtual AnalyzerInterface* createDefaultAnalyzer() = 0;

    virtual void applyToWidget(QWidget *widget) = 0;
    virtual void provideCallback(QSharedPointer<PluginCallback> pluginCallback) = 0;
    virtual QJsonObject getStateFromUi() = 0;
    virtual bool canRecallPluginState(const QJsonObject &pluginState) = 0;
    virtual bool setPluginStateInUi(const QJsonObject &pluginState) = 0;

    virtual QSharedPointer<const AnalyzerResult> analyzeBits(
            QSharedPointer<const BitContainer> container,
            const QJsonObject &recallablePluginState,
            QSharedPointer<ActionProgress> progressTracker) = 0;
    virtual void previewBits(QSharedPointer<BitContainerPreview> container) = 0;

};

Q_DECLARE_INTERFACE(AnalyzerInterface, "hobbits.AnalyzerInterface")

#endif // ANALYZERINTERFACE_H
