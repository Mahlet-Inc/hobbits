#ifndef SKIPOP_H
#define SKIPOP_H

#include "bitop.h"

class SkipOp : public BitOp
{
public:
    SkipOp(qint64 value);

    void apply(
            const Frame &inputFrame,
            QSharedPointer<BitArray> outputBits,
            qint64 &inputIdx,
            qint64 &outputIdx) override;

    qint64 inputStep(qint64 inputBits) const override;
    qint64 outputStep(qint64 inputBits) const override;

};

#endif // SKIPOP_H
