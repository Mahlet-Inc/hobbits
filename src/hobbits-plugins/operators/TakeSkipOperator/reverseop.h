#ifndef REVERSEOP_H
#define REVERSEOP_H

#include "bitop.h"


class ReverseOp : public BitOp
{
public:
    ReverseOp(qint64 value);

    void apply(
            const Frame &inputFrame,
            QSharedPointer<BitArray> outputBits,
            qint64 &inputIdx,
            qint64 &outputIdx) override;

    qint64 inputStep(qint64 inputBits) const override;
    qint64 outputStep(qint64 inputBits) const override;

};

#endif // REVERSEOP_H
