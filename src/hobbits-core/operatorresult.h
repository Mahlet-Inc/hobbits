#ifndef OPERATORRESULT_H
#define OPERATORRESULT_H

#include "parameters.h"
#include <QList>
#include <QSharedPointer>
#include "bitcontainer.h"
#include "hobbits-core_global.h"

/**
  * @brief The OperatorResult class contains the result of an OperatorInterface run
  *
  * \see OperatorInterface
*/
class HOBBITSCORESHARED_EXPORT OperatorResult
{
public:
    OperatorResult();

    OperatorResult* setOutputContainers(QList<QSharedPointer<BitContainer>> outputContainers);
    QList<QSharedPointer<BitContainer>> outputContainers() const;

    OperatorResult* setParameters(const Parameters &parameters);
    const Parameters parameters() const;

    bool hasNullParameters() const;
    QString errorString() const;

    static QSharedPointer<const OperatorResult> result(QList<QSharedPointer<BitContainer>> outputContainers, Parameters parameters);
    static QSharedPointer<const OperatorResult> error(QString error);

private:
    QList<QSharedPointer<BitContainer>> m_outputContainers;
    Parameters m_parameters;
    QString m_errorString;
};

#endif // OPERATORRESULT_H
