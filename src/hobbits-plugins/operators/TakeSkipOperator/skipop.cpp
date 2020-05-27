#include "skipop.h"

SkipOp::SkipOp(qint64 value) :
    BitOp(value)
{

}

void SkipOp::apply(
        const Frame &inputFrame,
        QSharedPointer<BitArray> outputBits,
        qint64 &inputIdx,
        qint64 &outputIdx)
{
    Q_UNUSED(outputBits)
    Q_UNUSED(outputIdx)
    inputIdx = inputIdx + m_value;
    if (inputIdx < m_value) {
        inputIdx = inputFrame.size();
    }
    else {
        inputIdx = qMin(inputIdx, inputFrame.size());
    }
}

qint64 SkipOp::inputStep(qint64 inputBits) const
{
    Q_UNUSED(inputBits)
    return m_value;
}

qint64 SkipOp::outputStep(qint64 inputBits) const
{
    Q_UNUSED(inputBits)
    return 0;
}
