#ifndef OPERATORINTERFACE_H
#define OPERATORINTERFACE_H

#include "pluginactionprogress.h"
#include "bitcontainer.h"
#include "operatorresult.h"
#include "bitcontainerpreview.h"
#include "parameterdelegate.h"
#include <QSharedPointer>
#include "hobbitsplugin.h"

/**
  * @brief Implementations of the OperatorInterface plugin interface process bit data into new bit data
  *
  * Operator plugins take one or more BitContainer inputs and create one or more BitContainer outputs.
  *
  * \see ParameterDelegate OperatorResult BitContainer ActionProgress
*/
class HOBBITSCORESHARED_EXPORT OperatorInterface : public virtual HobbitsPlugin
{
public:
    virtual ~OperatorInterface() = default;

    virtual OperatorInterface* createDefaultOperator() = 0;

    virtual QSharedPointer<ParameterDelegate> parameterDelegate() = 0;

    virtual int getMinInputContainers(const QJsonObject &parameters) = 0;
    virtual int getMaxInputContainers(const QJsonObject &parameters) = 0;

    virtual QSharedPointer<const OperatorResult> operateOnBits(
            QList<QSharedPointer<const BitContainer>> inputContainers,
            const QJsonObject &parameters,
            QSharedPointer<PluginActionProgress> progress) = 0;
};

Q_DECLARE_INTERFACE(OperatorInterface, "hobbits.OperatorInterface")

#endif // OPERATORINTERFACE_H
