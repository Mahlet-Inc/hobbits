#ifndef RANGESEQUENCE_H
#define RANGESEQUENCE_H

#include <QTemporaryFile>
#include <QQueue>
#include <QMutex>
#include "range.h"
#include "hobbits-core_global.h"

/**
  * @brief The RangeSequence class is a memory-friendly list of ordered, contiguous Ranges
  *
  * \see Range BitInfo
*/
class HOBBITSCORESHARED_EXPORT RangeSequence
{
public:
    RangeSequence();
    ~RangeSequence();

    static QSharedPointer<RangeSequence> fromConstantSize(qint64 constantSize, qint64 valueCount);
    static QSharedPointer<RangeSequence> fromOther(QSharedPointer<const RangeSequence> other);
    static QSharedPointer<RangeSequence> createEmpty();

    Range at(qint64 i) const;
    qint64 size() const;
    qint64 indexOf(qint64 value, Range indexBounds) const;

    void setConstantSize(qint64 constantSize, qint64 valueCount);
    void appendRange(qint64 rangeSize);

    qint64 getMaxSize() const;

    qint64 getValueCount() const;

private:
    void writeRange(qint64 i, qint64 end);
    Range readRange(qint64 i) const;
    qint64 * getLocation(qint64 i) const;

    void syncCacheWithFile() const;
    void resizeCache(int blockCount) const;
    void clearCache() const;

    qint64 m_valueCount;
    qint64 m_size;
    qint64 m_constantSize;
    qint64 m_maxSize;

    mutable QTemporaryFile m_dataFile;
    mutable QQueue<qint64> m_recentCacheAccess;
    mutable qint64 **m_dataCaches;
    mutable bool m_dirtyCache;
    mutable int m_dataCacheBlockCount;

    QMutex m_mutex;
    mutable QMutex m_cacheMutex;
};

#endif // RANGESEQUENCE_H
