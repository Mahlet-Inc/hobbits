#ifndef INVERTOP_H
#define INVERTOP_H

#include "bitop.h"


class InvertOp : public BitOp
{
public:
    InvertOp(int value);

    void apply(
            QSharedPointer<const BitArray> inputBits,
            QSharedPointer<BitArray> outputBits,
            int &inputIdx,
            int &outputIdx) override;

    int inputStep() const override;
    int outputStep() const override;

};

#endif // INVERTOP_H
