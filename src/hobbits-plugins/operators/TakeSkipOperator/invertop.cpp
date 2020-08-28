#include "invertop.h"

InvertOp::InvertOp(qint64 value) :
    BitOp(value)
{

}

void InvertOp::apply(
        const Frame &inputFrame,
        QSharedPointer<BitArray> outputBits,
        qint64 &inputIdx,
        qint64 &outputIdx)
{
    qint64 taken = inputFrame.copyBits(inputIdx, outputBits.data(), outputIdx, m_value, BitArray::Invert);
    inputIdx += taken;
    outputIdx += taken;
}

qint64 InvertOp::inputStep(qint64 inputBits) const
{
    return qMin(inputBits, m_value);
}

qint64 InvertOp::outputStep(qint64 inputBits) const
{
    return qMin(inputBits, m_value);
}
