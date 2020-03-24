#include "rangehighlight.h"

RangeHighlight::RangeHighlight()
{

}

RangeHighlight::RangeHighlight(QString category, QString label, Range range, QColor color) :
    m_category(category),
    m_label(label),
    m_range(range),
    m_color(color)
{

}

bool operator<(const RangeHighlight &a, const RangeHighlight &b)
{
    return a.range() < b.range();
}

QString RangeHighlight::label() const
{
    return m_label;
}

QString RangeHighlight::category() const
{
    return m_category;
}

Range RangeHighlight::range() const
{
    return m_range;
}

QColor RangeHighlight::color() const
{
    return m_color;
}

const QString VERSION_1 = "RangeHighlight v1";
QDataStream& HOBBITSCORESHARED_EXPORT operator<<(QDataStream& stream, const RangeHighlight& highlight)
{
    stream << VERSION_1;
    stream << highlight.category();
    stream << highlight.label();
    stream << highlight.range();
    stream << highlight.color();
}

QDataStream& HOBBITSCORESHARED_EXPORT operator>>(QDataStream& stream, RangeHighlight& highlight)
{
    QString version;
    stream >> version;
    if (version == VERSION_1) {
        stream >> highlight.m_category;
        stream >> highlight.m_label;
        stream >> highlight.m_range;
        stream >> highlight.m_color;
    }
    else {
        stream.setStatus(QDataStream::Status::ReadCorruptData);
        return stream;
    }
}
