#ifndef BITINFO_H
#define BITINFO_H

#include "frame.h"
#include "bitarray.h"
#include "rangehighlight.h"
#include <QVector>
#include <QHash>
#include <QSharedPointer>

class HOBBITSCORESHARED_EXPORT BitInfo
{
public:
    BitInfo();
    BitInfo(QSharedPointer<const BitArray> bits);

    void setBits(QSharedPointer<const BitArray> bits);
    void setFrames(QVector<Range> frames);
    void addHighlight(RangeHighlight highlight);
    void addHighlights(QList<RangeHighlight> highlights);
    void setMetadata(QString key, QVariant value);
    void clearHighlightCategory(QString category);

    qint64 maxFrameWidth() const;
    QVector<Frame> frames() const;
    QList<RangeHighlight> highlights(QString category) const;
    QList<QString> highlightCategories() const;
    bool containsHighlightCategory(QString category) const;
    QVariant metadata(QString key) const;

    int frameOffsetContaining(Range target) const;

    friend QDataStream& HOBBITSCORESHARED_EXPORT operator<<(QDataStream&, const BitInfo&);
    friend QDataStream& HOBBITSCORESHARED_EXPORT operator>>(QDataStream&, BitInfo&);

private:
    void initFrames();

    QSharedPointer<const BitArray> m_bits;
    qint64 m_maxFrameWidth;
    QVector<Range> m_ranges;
    QVector<Frame> m_frames;
    QHash<QString, QList<RangeHighlight>> m_rangeHighlights;
    QVariantHash m_metadata;
};


#endif // BITINFO_H
