#ifndef INVERTOP_H
#define INVERTOP_H

#include "bitop.h"


class InvertOp : public BitOp
{
public:
    InvertOp(qint64 value);

    void apply(
            const Frame &inputFrame,
            QSharedPointer<BitArray> outputBits,
            qint64 &inputIdx,
            qint64 &outputIdx) override;

    qint64 inputStep(qint64 inputBits) const override;
    qint64 outputStep(qint64 inputBits) const override;

};

#endif // INVERTOP_H
