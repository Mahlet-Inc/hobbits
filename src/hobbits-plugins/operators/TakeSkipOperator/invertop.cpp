#include "invertop.h"

InvertOp::InvertOp(qint64 value) :
    BitOp(value)
{

}

void InvertOp::apply(
        QSharedPointer<const BitArray> inputBits,
        QSharedPointer<BitArray> outputBits,
        qint64 &inputIdx,
        qint64 &outputIdx)
{
    for (int i = 0; i < m_value && inputIdx < inputBits->sizeInBits(); i++) {
        outputBits->set(outputIdx, !inputBits->at(inputIdx));
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
