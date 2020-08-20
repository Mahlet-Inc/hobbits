#ifndef RANGEHIGHLIGHT_H
#define RANGEHIGHLIGHT_H

#include <QString>
#include "range.h"
#include "hobbits-core_global.h"

class QColor;

class HOBBITSCORESHARED_EXPORT RangeHighlight
{
public:
    RangeHighlight() = default;
    ~RangeHighlight() = default;
    RangeHighlight(const RangeHighlight &) = default;
    RangeHighlight &operator=(const RangeHighlight &) = default;

    RangeHighlight(QString category, QString label, Range range, QColor color, QList<RangeHighlight> children = {});
    RangeHighlight(QString category, QString label, QList<RangeHighlight> children, QColor color);

    static RangeHighlight simple(QString category, QString label, Range range, unsigned int color);

    QString label() const;
    QString category() const;
    Range range() const;
    unsigned int rgbaColor() const;
    QColor color() const;
    QList<RangeHighlight> children() const;
    QList<RangeHighlight> allDescendants() const;

    friend QDataStream& operator<<(QDataStream&, const RangeHighlight&);
    friend QDataStream& operator>>(QDataStream&, RangeHighlight&);

private:
    QString m_category;
    QString m_label;
    Range m_range;
    unsigned int m_color;
    QList<RangeHighlight> m_children;
};

bool HOBBITSCORESHARED_EXPORT operator<(const RangeHighlight &a, const RangeHighlight &b);

Q_DECLARE_METATYPE(RangeHighlight);

#endif // RANGEHIGHLIGHT_H
