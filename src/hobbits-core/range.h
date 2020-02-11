#ifndef RANGE_H
#define RANGE_H

#include <QDataStream>

#include "hobbits-core_global.h"

class HOBBITSCORESHARED_EXPORT Range
{
public:
    Range();
    Range(int start, int end);
    Range(const Range &other) = default;

    bool operator<(const Range &other);

    int size() const;

    int start() const;
    int end() const;

    unsigned int compare(const Range&) const;

    Range getOverlap(const Range &other) const;

    void translate(int offset);

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
    int m_start;
    int m_end;
};

#endif // RANGE_H
