#ifndef OPERATORINTERFACE_H
#define OPERATORINTERFACE_H

#include "actionprogress.h"
#include "bitcontainer.h"
#include "operatorresult.h"
#include "plugincallback.h"
#include <QSharedPointer>

class OperatorInterface
{
public:
    virtual ~OperatorInterface()
    {
    }

    virtual QString getName() = 0;
    virtual OperatorInterface* createDefaultOperator() = 0;

    virtual void applyToWidget(QWidget *widget) = 0;
    virtual void provideCallback(QSharedPointer<PluginCallback> pluginCallback) = 0;
    virtual QJsonObject getStateFromUi() = 0;
    virtual bool canRecallPluginState(const QJsonObject &pluginState) = 0;
    virtual bool setPluginStateInUi(const QJsonObject &pluginState) = 0;

    virtual int getMinInputContainers() = 0;
    virtual int getMaxInputContainers() = 0;
    virtual QSharedPointer<const OperatorResult> operateOnContainers(
            QList<QSharedPointer<const BitContainer>> inputContainers,
            const QJsonObject &recallablePluginState,
            QSharedPointer<ActionProgress> progressTracker) = 0;

};

Q_DECLARE_INTERFACE(OperatorInterface, "hobbits.OperatorInterface.3")

#endif // OPERATORINTERFACE_H
