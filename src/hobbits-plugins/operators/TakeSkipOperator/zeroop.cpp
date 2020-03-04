#include "zeroop.h"

ZeroOp::ZeroOp(qint64 value) :
    BitOp(value)
{

}

void ZeroOp::apply(
        QSharedPointer<const BitArray> inputBits,
        QSharedPointer<BitArray> outputBits,
        qint64 &inputIdx,
        qint64 &outputIdx)
{
    Q_UNUSED(inputBits)
    Q_UNUSED(inputIdx)
    for (qint64 i = 0; i < m_value; i++) {
        outputBits->set(outputIdx, false);
        outputIdx++;
    }
}

qint64 ZeroOp::inputStep(qint64 inputBits) const
{
    return 0;
}

qint64 ZeroOp::outputStep(qint64 inputBits) const
{
    return m_value;
}
