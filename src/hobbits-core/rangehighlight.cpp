#include "rangehighlight.h"

RangeHighlight::RangeHighlight(QString category, QString label, Range range, quint32 color, QList<RangeHighlight> children, QStringList tags) :
    m_category(category),
    m_label(label),
    m_range(range),
    m_color(color),
    m_children(children),
    m_tags(tags)
{

}

RangeHighlight::RangeHighlight(QString category, QString label, QList<RangeHighlight> children, quint32 color, QStringList tags) :
    m_category(category),
    m_label(label),
    m_color(color),
    m_children(children),
    m_tags(tags)
{
    std::sort(m_children.begin(), m_children.end());
    if (m_children.isEmpty()) {
        return;
    }
    m_range = Range(m_children.first().range().start(), m_children.last().range().end());
}

RangeHighlight RangeHighlight::simple(QString category, QString label, Range range, quint32 color, QStringList tags)
{
    return RangeHighlight(category, label, range, color, {}, tags);
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

quint32 RangeHighlight::color() const
{
    return m_color;
}

QList<RangeHighlight> RangeHighlight::children() const
{
    return m_children;
}

QList<RangeHighlight> RangeHighlight::allDescendants() const
{
    QList<RangeHighlight> all;
    for (auto child : children()) {
        all.append(child);
        all.append(child.allDescendants());
    }
    return all;
}

QStringList RangeHighlight::tags() const
{
    return m_tags;
}

const QString VERSION_1 = "RangeHighlight v1";
const QString VERSION_2 = "RangeHighlight v2";
const QString VERSION_3 = "RangeHighlight v3";
const QString VERSION_4 = "RHv4";
QDataStream& operator<<(QDataStream& stream, const RangeHighlight& highlight)
{
    stream << VERSION_4;
    stream << highlight.category();
    stream << highlight.label();
    stream << highlight.range();
    stream << highlight.color();
    stream << highlight.children();
    stream << highlight.tags();
    return stream;
}

QDataStream& operator>>(QDataStream& stream, RangeHighlight& highlight)
{
    QString version;
    stream >> version;
    if (version == VERSION_3 || version == VERSION_4) {
        stream >> highlight.m_category;
        stream >> highlight.m_label;
        stream >> highlight.m_range;
        stream >> highlight.m_color;
        stream >> highlight.m_children;
        if (version == VERSION_4) {
            stream >> highlight.m_tags;
        }
        return stream;
    }
    else {
        stream.setStatus(QDataStream::Status::ReadCorruptData);
        return stream;
    }
}
