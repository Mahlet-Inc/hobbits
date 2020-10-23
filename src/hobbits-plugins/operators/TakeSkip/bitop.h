#ifndef BITOP_H
#define BITOP_H

#include "frame.h"
#include <bitarray.h>
#include <QSharedPointer>

class BitOp
{
public:
    BitOp(qint64 value);

    virtual ~BitOp();

    virtual void apply(
            const Frame &inputFrame,
            QSharedPointer<BitArray> outputBits,
            qint64 &inputIdx,
            qint64 &outputIdx) = 0;

    static QList<QSharedPointer<BitOp>> parseOps(QString opString);

    virtual qint64 inputStep(qint64 inputBits) const = 0;
    virtual qint64 outputStep(qint64 inputBits) const = 0;

protected:
    qint64 m_value;
};

#endif // BITOP_H
