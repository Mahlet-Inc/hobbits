#ifndef BITINFO_H
#define BITINFO_H

#include <QObject>
#include "frame.h"
#include "bitarray.h"
#include "rangehighlight.h"
#include <QVector>
#include <QHash>
#include <QSharedPointer>
#include <QVariant>
#include <QMutex>

class HOBBITSCORESHARED_EXPORT BitInfo : public QObject
{
    Q_OBJECT

public:
    explicit BitInfo(QObject *parent = nullptr);
    BitInfo(QSharedPointer<const BitArray> bits, QObject *parent = nullptr);
    QSharedPointer<BitInfo> copyMetadata() const;

    void setBits(QSharedPointer<const BitArray> bits);
    void setFrames(QVector<Range> frames);
    void addHighlight(RangeHighlight highlight);
    void addHighlights(QList<RangeHighlight> highlights);
    void setMetadata(QString key, QVariant value);
    void clearHighlightCategory(QString category);

    qint64 maxFrameWidth() const;
    QVector<Frame> frames() const;
    QList<RangeHighlight> highlights(QString category) const;
    QList<RangeHighlight> highlights(QString category, QString label) const;
    QList<QString> highlightCategories() const;
    bool containsHighlightCategory(QString category) const;
    QVariant metadata(QString key) const;

    int frameOffsetContaining(Range target, Range frameRange = Range()) const;

    friend QDataStream& operator<<(QDataStream&, const BitInfo&);
    friend QDataStream& operator>>(QDataStream&, BitInfo&);

Q_SIGNALS:
    void changed();

private:
    void initFrames();

    QSharedPointer<const BitArray> m_bits;
    qint64 m_maxFrameWidth;
    QVector<Range> m_ranges;
    QVector<Frame> m_frames;
    QHash<QString, QList<RangeHighlight>> m_rangeHighlights;
    QVariantHash m_metadata;
    QMutex m_mutex;
};


#endif // BITINFO_H
