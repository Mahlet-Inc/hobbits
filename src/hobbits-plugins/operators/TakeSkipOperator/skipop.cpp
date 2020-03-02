#include "skipop.h"

SkipOp::SkipOp(qint64 value) :
    BitOp(value)
{

}

void SkipOp::apply(
        QSharedPointer<const BitArray> inputBits,
        QSharedPointer<BitArray> outputBits,
        qint64 &inputIdx,
        qint64 &outputIdx)
{
    Q_UNUSED(outputBits)
    Q_UNUSED(outputIdx)
    inputIdx += m_value;
    inputIdx = qMin(inputIdx, inputBits->sizeInBits());
}

qint64 SkipOp::inputStep(qint64 inputBits) const
{
    return m_value;
}

qint64 SkipOp::outputStep(qint64 inputBits) const
{
    return 0;
}
