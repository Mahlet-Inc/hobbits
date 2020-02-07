#include "range.h"

#include <QString>

Range::Range() :
    m_start(0),
    m_end(0)
{

}

Range::Range(int start, int end) :
    m_start(start),
    m_end(end)
{

}

bool Range::operator<(const Range &other)
{
    return m_start < other.start();
}

int Range::size() const
{
    return m_end - m_start + 1;
}

int Range::start() const
{
    return m_start;
}

int Range::end() const
{
    return m_end;
}

unsigned int Range::compare(const Range &other) const
{
    unsigned int result = Range::Unknown;

    if (other.start() == this->start() && other.end() == this->end()) {
        result |= Range::Equal;
    }

    if ((other.start() >= this->start() && other.start() <= this->end())
        || (other.end() >= this->start() && other.end() <= this->end())
        || (this->start() >= other.start() && this->start() <= other.end())
        || (this->end() >= other.start() && this->end() <= other.end())) {
        result |= Range::Overlapping;
    }
    else {
        result |= Range::Separate;
    }

    if (other.end() > this->end()) {
        result |= Range::After;
    }

    if (other.start() < this->start()) {
        result |= Range::Before;
    }

    return result;
}

Range Range::getOverlap(const Range &other) const
{
    unsigned int comparison = this->compare(other);
    Q_ASSERT_X(comparison & Range::Overlapping, "Frame", "Cannot get overlap for non-overlapping frames");
    int start = other.start();
    int end = other.end();
    if (comparison & Range::Before) {
        start = this->start();
    }
    if (comparison & Range::After) {
        end = this->end();
    }

    return Range(start, end);
}

void Range::translate(int offset)
{
    m_start += offset;
    m_end += offset;
}

QDataStream& operator<<(QDataStream &stream, const Range &range)
{
    stream << range.m_start;
    stream << range.m_end;

    return stream;
}

QDataStream& operator>>(QDataStream &stream, Range &range)
{
    stream >> range.m_start;
    stream >> range.m_end;

    return stream;
}
