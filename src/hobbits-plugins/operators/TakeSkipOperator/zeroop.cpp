#include "zeroop.h"

ZeroOp::ZeroOp(int value) :
    BitOp(value)
{

}

void ZeroOp::apply(
        QSharedPointer<const BitArray> inputBits,
        QSharedPointer<BitArray> outputBits,
        int &inputIdx,
        int &outputIdx)
{
    Q_UNUSED(inputBits)
    Q_UNUSED(inputIdx)
    for (int i = 0; i < m_value; i++) {
        outputBits->set(outputIdx, false);
        outputIdx++;
    }
}

int ZeroOp::inputStep() const
{
    return 0;
}

int ZeroOp::outputStep() const
{
    return m_value;
}
