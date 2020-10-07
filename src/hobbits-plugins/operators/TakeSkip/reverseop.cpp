#include "reverseop.h"

ReverseOp::ReverseOp(qint64 value) :
    BitOp(value)
{

}

void ReverseOp::apply(
        const Frame &inputFrame,
        QSharedPointer<BitArray> outputBits,
        qint64 &inputIdx,
        qint64 &outputIdx)
{
    for (qint64 i = m_value - 1; i >= 0 && inputIdx + i < inputFrame.size(); i--) {
        outputBits->set(outputIdx, inputFrame.at(inputIdx + i));
        outputIdx++;
    }
    inputIdx = qMin(inputFrame.size(), inputIdx + m_value);
}

qint64 ReverseOp::inputStep(qint64 inputBits) const
{
    return qMin(inputBits, m_value);
}

qint64 ReverseOp::outputStep(qint64 inputBits) const
{
    return qMin(inputBits, m_value);
}
