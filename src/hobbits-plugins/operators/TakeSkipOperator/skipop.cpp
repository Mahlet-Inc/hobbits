#include "skipop.h"

SkipOp::SkipOp(int value) :
    BitOp(value)
{

}

void SkipOp::apply(
        QSharedPointer<const BitArray> inputBits,
        QSharedPointer<BitArray> outputBits,
        int &inputIdx,
        int &outputIdx)
{
    Q_UNUSED(outputBits)
    Q_UNUSED(outputIdx)
    inputIdx += m_value;
    inputIdx = qMin(inputIdx, inputBits->size());
}

int SkipOp::inputStep() const
{
    return m_value;
}

int SkipOp::outputStep() const
{
    return 0;
}
