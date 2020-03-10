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
    for (int i = 0; i < m_value && inputIdx < inputFrame.size(); i++) {
        outputBits->set(outputIdx, !inputFrame.at(inputIdx));
        inputIdx++;
        outputIdx++;
    }
}

qint64 InvertOp::inputStep(qint64 inputBits) const
{
    return qMin(inputBits, m_value);
}

qint64 InvertOp::outputStep(qint64 inputBits) const
{
    return qMin(inputBits, m_value);
}
