#include "oneop.h"

OneOp::OneOp(int value) :
    BitOp(value)
{

}

void OneOp::apply(
        QSharedPointer<const BitArray> inputBits,
        QSharedPointer<BitArray> outputBits,
        int &inputIdx,
        int &outputIdx)
{
    Q_UNUSED(inputBits)
    Q_UNUSED(inputIdx)
    for (int i = 0; i < m_value; i++) {
        outputBits->set(outputIdx, true);
        outputIdx++;
    }
}

int OneOp::inputStep() const
{
    return 0;
}

int OneOp::outputStep() const
{
    return m_value;
}
