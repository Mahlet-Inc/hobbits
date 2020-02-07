#ifndef BITOP_H
#define BITOP_H

#include <bitarray.h>
#include <QSharedPointer>

class BitOp
{
public:
    BitOp(int value);

    virtual ~BitOp();

    virtual void apply(
            QSharedPointer<const BitArray> inputBits,
            QSharedPointer<BitArray> outputBits,
            int &inputIdx,
            int &outputIdx) = 0;

    static QList<QSharedPointer<BitOp>> parseOps(QString opString);

    virtual int inputStep() const = 0;
    virtual int outputStep() const = 0;

protected:
    int m_value;
};

#endif // BITOP_H
