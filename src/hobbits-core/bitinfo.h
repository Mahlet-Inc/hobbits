#ifndef BITINFO_H
#define BITINFO_H

#include <QObject>
#include "rangehighlight.h"
#include <QVector>
#include <QHash>
#include <QSharedPointer>
#include <QVariant>
#include <QMutex>
#include "rangesequence.h"

class BitContainer;

/**
  * @brief The BitInfo class provides metadata for bitwise data
  *
  * \see BitContainer RangeSequence RangeHighlight
*/
class HOBBITSCORESHARED_EXPORT BitInfo : public QObject
{
    Q_OBJECT

public:
    static QSharedPointer<BitInfo> create(qint64 bitLength = 0, QSharedPointer<const BitInfo> other = QSharedPointer<const BitInfo>(), bool clearFrames = false);
    static QSharedPointer<BitInfo> copyFromContainer(QSharedPointer<const BitContainer> container, bool clearFrames = false);

    void setFrames(QSharedPointer<const RangeSequence> frames);
    void addHighlight(RangeHighlight highlight);
    void addHighlights(QList<RangeHighlight> highlights);
    void setMetadata(QString key, QVariant value);
    void clearHighlightCategory(QString category);

    qint64 maxFrameWidth() const;
    qint64 frameCount() const;
    QSharedPointer<const RangeSequence> frames() const;
    QList<RangeHighlight> highlights(QString category) const;
    QList<RangeHighlight> highlights(QString category, QString label) const;
    QList<QString> highlightCategories() const;
    bool containsHighlightCategory(QString category) const;
    QVariant metadata(QString key) const;
    QList<QString> metadataKeys() const;

    qint64 frameOffsetContaining(qint64 value, Range indexBounds = Range()) const;

    friend QDataStream& operator<<(QDataStream&, const BitInfo&);
    friend QDataStream& operator>>(QDataStream&, BitInfo&);

Q_SIGNALS:
    void changed();

private:
    explicit BitInfo();

    QSharedPointer<RangeSequence> m_frames;
    QHash<QString, QList<RangeHighlight>> m_rangeHighlights;
    QVariantHash m_metadata;
    QMutex m_mutex;
};


#endif // BITINFO_H
