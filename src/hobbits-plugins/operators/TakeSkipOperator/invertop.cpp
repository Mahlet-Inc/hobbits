#include "invertop.h"

InvertOp::InvertOp(int value) :
    BitOp(value)
{

}

void InvertOp::apply(
        QSharedPointer<const BitArray> inputBits,
        QSharedPointer<BitArray> outputBits,
        int &inputIdx,
        int &outputIdx)
{
    for (int i = 0; i < m_value && inputIdx < inputBits->size(); i++) {
        outputBits->set(outputIdx, !inputBits->at(inputIdx));
        inputIdx++;
        outputIdx++;
    }
}

int InvertOp::inputStep() const
{
    return m_value;
}

int InvertOp::outputStep() const
{
    return m_value;
}
