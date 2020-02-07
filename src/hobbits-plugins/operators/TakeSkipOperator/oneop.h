#ifndef ONEOP_H
#define ONEOP_H

#include "bitop.h"


class OneOp : public BitOp
{
public:
    OneOp(int value);

    void apply(
            QSharedPointer<const BitArray> inputBits,
            QSharedPointer<BitArray> outputBits,
            int &inputIdx,
            int &outputIdx) override;

    int inputStep() const override;
    int outputStep() const override;

};

#endif // ONEOP_H
