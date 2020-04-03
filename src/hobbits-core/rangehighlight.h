#ifndef RANGEHIGHLIGHT_H
#define RANGEHIGHLIGHT_H

#include <QColor>
#include <QString>
#include "range.h"
#include "hobbits-core_global.h"

class HOBBITSCORESHARED_EXPORT RangeHighlight
{
public:
    RangeHighlight();
    RangeHighlight(QString category, QString label, Range range, QColor color);

    QString label() const;
    QString category() const;
    Range range() const;
    QColor color() const;

    friend QDataStream& operator<<(QDataStream&, const RangeHighlight&);
    friend QDataStream& operator>>(QDataStream&, RangeHighlight&);

private:
    QString m_category;
    QString m_label;
    Range m_range;
    QColor m_color;
};

bool HOBBITSCORESHARED_EXPORT operator<(const RangeHighlight &a, const RangeHighlight &b);


#endif // RANGEHIGHLIGHT_H
