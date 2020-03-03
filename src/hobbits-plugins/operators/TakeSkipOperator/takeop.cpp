#include "takeop.h"

TakeOp::TakeOp(qint64 value) :
    BitOp(value)
{

}

void TakeOp::apply(
        QSharedPointer<const BitArray> inputBits,
        QSharedPointer<BitArray> outputBits,
        qint64 &inputIdx,
        qint64 &outputIdx)
{
    for (qint64 i = 0; i < m_value && inputIdx < inputBits->sizeInBits(); i++) {
        outputBits->set(outputIdx, inputBits->at(inputIdx));
        inputIdx++;
        outputIdx++;
    }
}

qint64 TakeOp::inputStep(qint64 inputBits) const
{
    return qMin(inputBits, m_value);
}

qint64 TakeOp::outputStep(qint64 inputBits) const
{
    return qMin(inputBits, m_value);
}
