#ifndef ONEOP_H
#define ONEOP_H

#include "bitop.h"


class OneOp : public BitOp
{
public:
    OneOp(qint64 value);

    void apply(
            const Frame &inputFrame,
            QSharedPointer<BitArray> outputBits,
            qint64 &inputIdx,
            qint64 &outputIdx) override;

    qint64 inputStep(qint64 inputBits) const override;
    qint64 outputStep(qint64 inputBits) const override;

};

#endif // ONEOP_H
