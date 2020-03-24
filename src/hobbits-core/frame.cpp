#include "frame.h"

#include <QString>

Frame::Frame() :
    Range(-1, -1)
{

}

Frame::Frame(QSharedPointer<const BitArray> bits, qint64 start, qint64 end) :
    Range(start, end),
    m_bits(bits)
{

}

Frame::Frame(QSharedPointer<const BitArray> bits, Range range) :
    Range(range),
    m_bits(bits)
{

}

bool Frame::at(qint64 i) const
{
    Q_ASSERT_X(
            i >= 0 && i < this->size(),
            "Frame",
            QStringLiteral("Index %1 is not valid").arg(i).toLocal8Bit().data());
    return m_bits->at(this->start() + i);
}
