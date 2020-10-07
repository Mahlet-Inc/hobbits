#ifndef RANGEHIGHLIGHT_H
#define RANGEHIGHLIGHT_H

#include <QString>
#include "range.h"
#include "hobbits-core_global.h"

/**
  * @brief The RangeHighlight class defines a Range with some additional, display-oriented metadata
  *
  * \see Range BitInfo
*/
class HOBBITSCORESHARED_EXPORT RangeHighlight
{
public:
    RangeHighlight() = default;
    ~RangeHighlight() = default;
    RangeHighlight(const RangeHighlight &) = default;
    RangeHighlight &operator=(const RangeHighlight &) = default;

    RangeHighlight(QString category, QString label, Range range, quint32 color, QList<RangeHighlight> children = {});
    RangeHighlight(QString category, QString label, QList<RangeHighlight> children, quint32 color);

    static RangeHighlight simple(QString category, QString label, Range range, quint32 color);

    QString label() const;
    QString category() const;
    Range range() const;
    quint32 color() const;
    QList<RangeHighlight> children() const;
    QList<RangeHighlight> allDescendants() const;

    friend QDataStream& operator<<(QDataStream&, const RangeHighlight&);
    friend QDataStream& operator>>(QDataStream&, RangeHighlight&);

private:
    QString m_category;
    QString m_label;
    Range m_range;
    quint32 m_color;
    QList<RangeHighlight> m_children;
};

bool HOBBITSCORESHARED_EXPORT operator<(const RangeHighlight &a, const RangeHighlight &b);

Q_DECLARE_METATYPE(RangeHighlight);

#endif // RANGEHIGHLIGHT_H
