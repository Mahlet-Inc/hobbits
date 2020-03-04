#include "oneop.h"

OneOp::OneOp(qint64 value) :
    BitOp(value)
{

}

void OneOp::apply(
        QSharedPointer<const BitArray> inputBits,
        QSharedPointer<BitArray> outputBits,
        qint64 &inputIdx,
        qint64 &outputIdx)
{
    Q_UNUSED(inputBits)
    Q_UNUSED(inputIdx)
    for (int i = 0; i < m_value; i++) {
        outputBits->set(outputIdx, true);
        outputIdx++;
    }
}

qint64 OneOp::inputStep(qint64 inputBits) const
{
    return 0;
}

qint64 OneOp::outputStep(qint64 inputBits) const
{
    return m_value;
}
