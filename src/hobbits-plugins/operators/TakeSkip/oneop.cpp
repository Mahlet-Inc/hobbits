#include "oneop.h"

OneOp::OneOp(qint64 value) :
    BitOp(value)
{

}

void OneOp::apply(
        const Frame &inputFrame,
        QSharedPointer<BitArray> outputBits,
        qint64 &inputIdx,
        qint64 &outputIdx)
{
    Q_UNUSED(inputFrame)
    Q_UNUSED(inputIdx)
    for (int i = 0; i < m_value; i++) {
        outputBits->set(outputIdx, true);
        outputIdx++;
    }
}

qint64 OneOp::inputStep(qint64 inputBits) const
{
    Q_UNUSED(inputBits)
    return 0;
}

qint64 OneOp::outputStep(qint64 inputBits) const
{
    Q_UNUSED(inputBits)
    return m_value;
}
