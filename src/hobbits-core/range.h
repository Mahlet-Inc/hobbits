#ifndef RANGE_H
#define RANGE_H

#include <QDataStream>
#include "hobbits-core_global.h"

/**
  * @brief The Range class defines a contiguous range with integer precision
  *
  * \see Frame RangeHighlight RangeSequence
*/
class HOBBITSCORESHARED_EXPORT Range
{
public:
    Range();
    Range(qint64 start, qint64 end);
    Range(const Range &other) = default;

    qint64 size() const;

    qint64 start() const;
    qint64 end() const;

    bool isNull() const;

    unsigned int compare(const Range&) const;

    Range getOverlap(const Range &other) const;

    void translate(qint64 offset);

    enum Comparison {
        Unknown = 0x00,
        Separate = 0x01,
        Overlapping = 0x02,
        Before = 0x04,
        After = 0x08,
        Within = 0x10,
        Around = 0x20,
        Equal = 0x40
    };

    friend HOBBITSCORESHARED_EXPORT QDataStream& operator<<(QDataStream&, const Range&);
    friend HOBBITSCORESHARED_EXPORT QDataStream& operator>>(QDataStream&, Range&);

private:
    qint64 m_start;
    qint64 m_end;
    bool m_null;
};

bool HOBBITSCORESHARED_EXPORT operator<(const Range &a, const Range &b);

#endif // RANGE_H
