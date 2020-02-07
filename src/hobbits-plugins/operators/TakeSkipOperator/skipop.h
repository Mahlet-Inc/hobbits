#ifndef SKIPOP_H
#define SKIPOP_H

#include "bitop.h"

class SkipOp : public BitOp
{
public:
    SkipOp(int value);

    void apply(
            QSharedPointer<const BitArray> inputBits,
            QSharedPointer<BitArray> outputBits,
            int &inputIdx,
            int &outputIdx) override;

    int inputStep() const override;
    int outputStep() const override;

};

#endif // SKIPOP_H
