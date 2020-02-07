#ifndef TAKEOP_H
#define TAKEOP_H

#include "bitop.h"

class TakeOp : public BitOp
{
public:
    TakeOp(int value);

    void apply(
            QSharedPointer<const BitArray> inputBits,
            QSharedPointer<BitArray> outputBits,
            int &inputIdx,
            int &outputIdx) override;

    int inputStep() const override;
    int outputStep() const override;

};

#endif // TAKEOP_H
