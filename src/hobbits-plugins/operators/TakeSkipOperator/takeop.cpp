#include "takeop.h"

TakeOp::TakeOp(int value) :
    BitOp(value)
{

}

void TakeOp::apply(
        QSharedPointer<const BitArray> inputBits,
        QSharedPointer<BitArray> outputBits,
        int &inputIdx,
        int &outputIdx)
{
    for (int i = 0; i < m_value && inputIdx < inputBits->size(); i++) {
        outputBits->set(outputIdx, inputBits->at(inputIdx));
        inputIdx++;
        outputIdx++;
    }
}

int TakeOp::inputStep() const
{
    return m_value;
}

int TakeOp::outputStep() const
{
    return m_value;
}
