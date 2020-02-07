#ifndef FRAME_H
#define FRAME_H

#include "bitarray.h"
#include "range.h"
#include <QSharedPointer>

class Frame : public Range
{
public:
    Frame(QSharedPointer<const BitArray> bits, int start, int end);
    Frame(QSharedPointer<const BitArray> bits, Range range);
    Frame(const Frame &other) = default;

    bool at(int i) const;

private:
    QSharedPointer<const BitArray> m_bits;
};

#endif // FRAME_H
