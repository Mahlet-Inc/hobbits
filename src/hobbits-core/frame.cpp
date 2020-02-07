#include "frame.h"

#include <QString>

Frame::Frame(QSharedPointer<const BitArray> bits, int start, int end) :
    Range(start, end),
    m_bits(bits)
{

}

Frame::Frame(QSharedPointer<const BitArray> bits, Range range) :
    Range(range),
    m_bits(bits)
{

}

bool Frame::at(int i) const
{
    Q_ASSERT_X(
            i >= 0 && i < this->size(),
            "Frame",
            QStringLiteral("Index %1 is not valid").arg(i).toLocal8Bit().data());
    return m_bits->at(this->start() + i);
}
