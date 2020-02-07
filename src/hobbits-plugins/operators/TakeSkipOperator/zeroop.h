#ifndef ZEROOP_H
#define ZEROOP_H

#include "bitop.h"


class ZeroOp : public BitOp
{
public:
    ZeroOp(int value);

    void apply(
            QSharedPointer<const BitArray> inputBits,
            QSharedPointer<BitArray> outputBits,
            int &inputIdx,
            int &outputIdx) override;

    int inputStep() const override;
    int outputStep() const override;

};

#endif // ZEROOP_H
