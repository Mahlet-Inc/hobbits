#ifndef OPERATORRESULT_H
#define OPERATORRESULT_H

#include <QJsonObject>
#include <QList>
#include <QSharedPointer>

#include "bitcontainer.h"

#include "hobbits-core_global.h"

class HOBBITSCORESHARED_EXPORT OperatorResult
{
public:
    OperatorResult();

    OperatorResult* setOutputContainers(QList<QSharedPointer<BitContainer>> outputContainers);
    QList<QSharedPointer<BitContainer>> getOutputContainers() const;

    OperatorResult* setPluginState(QJsonObject pluginState);
    const QJsonObject getPluginState() const;

    static QSharedPointer<const OperatorResult> result(QList<QSharedPointer<BitContainer>> outputContainers, QJsonObject pluginState);
    static QSharedPointer<const OperatorResult> error(QString error);

private:
    QList<QSharedPointer<BitContainer>> m_outputContainers;

    QJsonObject m_pluginState;
};

#endif // OPERATORRESULT_H
