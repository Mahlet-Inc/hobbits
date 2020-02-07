#ifndef REVERSEOP_H
#define REVERSEOP_H

#include "bitop.h"


class ReverseOp : public BitOp
{
public:
    ReverseOp(int value);

    void apply(
            QSharedPointer<const BitArray> inputBits,
            QSharedPointer<BitArray> outputBits,
            int &inputIdx,
            int &outputIdx) override;

    int inputStep() const override;
    int outputStep() const override;

};

#endif // REVERSEOP_H
