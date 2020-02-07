#include "reverseop.h"

ReverseOp::ReverseOp(int value) :
    BitOp(value)
{

}

void ReverseOp::apply(
        QSharedPointer<const BitArray> inputBits,
        QSharedPointer<BitArray> outputBits,
        int &inputIdx,
        int &outputIdx)
{
    for (int i = m_value - 1; i >= 0 && inputIdx + i < inputBits->size(); i--) {
        outputBits->set(outputIdx, inputBits->at(inputIdx + i));
        outputIdx++;
    }
    inputIdx = qMin(inputBits->size(), inputIdx + m_value);
}

int ReverseOp::inputStep() const
{
    return m_value;
}

int ReverseOp::outputStep() const
{
    return m_value;
}
