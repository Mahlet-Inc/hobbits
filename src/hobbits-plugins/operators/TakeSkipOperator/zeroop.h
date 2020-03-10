#ifndef ZEROOP_H
#define ZEROOP_H

#include "bitop.h"


class ZeroOp : public BitOp
{
public:
    ZeroOp(qint64 value);

    void apply(
            const Frame &inputFrame,
            QSharedPointer<BitArray> outputBits,
            qint64 &inputIdx,
            qint64 &outputIdx) override;

    qint64 inputStep(qint64 inputBits) const override;
    qint64 outputStep(qint64 inputBits) const override;

};

#endif // ZEROOP_H
