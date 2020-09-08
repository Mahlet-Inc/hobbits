#include "takeop.h"

TakeOp::TakeOp(qint64 value) :
    BitOp(value)
{

}

void TakeOp::apply(
        const Frame &inputFrame,
        QSharedPointer<BitArray> outputBits,
        qint64 &inputIdx,
        qint64 &outputIdx)
{
    // copying into an empty bit array, so "Or" can be used instead of "Copy"
    qint64 taken = inputFrame.copyBits(inputIdx, outputBits.data(), outputIdx, m_value, BitArray::Or);
    inputIdx += taken;
    outputIdx += taken;
}

qint64 TakeOp::inputStep(qint64 inputBits) const
{
    return qMin(inputBits, m_value);
}

qint64 TakeOp::outputStep(qint64 inputBits) const
{
    return qMin(inputBits, m_value);
}
