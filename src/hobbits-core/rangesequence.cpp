#include "rangesequence.h"
#include <QMutexLocker>
#include <QSharedPointer>

#define CACHE_CHUNK_64_SIZE (1000ll * 1000ll)
#define CACHE_CHUNK_BYTE_SIZE (CACHE_CHUNK_64_SIZE * 8ll)
#define MAX_ACTIVE_CACHE_CHUNKS 5

RangeSequence::RangeSequence() :
    m_valueCount(0),
    m_size(0),
    m_constantSize(0),
    m_maxSize(0),
    m_dataCaches(nullptr),
    m_dirtyCache(false),
    m_dataCacheBlockCount(0)
{
}

RangeSequence::~RangeSequence()
{
    clearCache();
}

QSharedPointer<RangeSequence> RangeSequence::fromConstantSize(qint64 constantSize, qint64 valueCount)
{
    QSharedPointer<RangeSequence> sequence(new RangeSequence());
    sequence->setConstantSize(constantSize, valueCount);
    return sequence;
}

QSharedPointer<RangeSequence> RangeSequence::fromOther(QSharedPointer<RangeSequence> other)
{
    QSharedPointer<RangeSequence> sequence(new RangeSequence());
    if (other->m_size < 1) {
        return sequence;
    }
    if (other->m_constantSize > 0) {
        sequence->setConstantSize(other->m_constantSize, other->m_valueCount);
        return sequence;
    }

    sequence->m_size = other->m_size;
    sequence->m_maxSize = other->m_maxSize;
    sequence->m_valueCount = other->m_valueCount;

    if (other->m_dataCacheBlockCount > 0) {
        char* buffer = new char[CACHE_CHUNK_BYTE_SIZE];
        other->syncCacheWithFile();
        other->m_dataFile.seek(0);
        sequence->resizeCache(other->m_dataCacheBlockCount);
        while (other->m_dataFile.bytesAvailable() > 0) {
            qint64 bytes = other->m_dataFile.read(buffer, CACHE_CHUNK_BYTE_SIZE);
            if (bytes < 1) {
                break;
            }
            sequence->m_dataFile.write(buffer, bytes);
        }
        delete[] buffer;
    }

    return sequence;
}

QSharedPointer<RangeSequence> RangeSequence::createEmpty()
{
    return QSharedPointer<RangeSequence>(new RangeSequence());
}

Range RangeSequence::at(qint64 i) const
{
    if (i < 0 || i >= m_size) {
        throw std::invalid_argument(QString("Invalid range index '%1'").arg(i).toStdString());
    }

    if (m_constantSize > 0) {
        qint64 start = i*m_constantSize;
        return Range(start, qMin(start + m_constantSize - 1, m_valueCount - 1));
    }

    return readRange(i);
}

qint64 RangeSequence::size() const
{
    return m_size;
}

qint64 RangeSequence::getMaxSize() const
{
    return m_maxSize;
}

qint64 RangeSequence::indexOf(qint64 value, Range indexBounds) const
{
    if (value < 0 || value >= m_valueCount) {
        return -1;
    }

    if (m_constantSize > 0) {
        return value / m_constantSize;
    }

    if (!indexBounds.isNull()) {
        if (indexBounds.end() >= m_size || indexBounds.start() < 0) {
            return -1;
        }
    }
    else {
        indexBounds = Range(0, m_size - 1);
    }

    if (indexBounds.size() < 50) {
        for (qint64 i = indexBounds.start(); i <= indexBounds.end(); i++) {
            Range r = at(i);
            if (r.start() <= value && r.end() >= value) {
                return int(i);
            }
        }
        return -1;
    }

    qint64 offset = indexBounds.start() + indexBounds.size() / 2;
    qint64 index = offset;
    int countdown = 7;
    qint64 lowNo = indexBounds.start() - 1;
    while (true) {
        Range r = at(index);
        if (r.start() <= value && r.end() >= value) {
            if (index > lowNo + 1) {
                qint64 earlierIndex = indexOf(value, Range(lowNo + 1, index - 1));
                if (earlierIndex >= 0) {
                    return earlierIndex;
                }
            }
            return int(index);
        }

        if (countdown < 1) {
            return -1;
        }

        offset /= 2;
        if (offset < 1) {
            offset = 1;
            countdown--;
        }

        if (value > r.end()) {
            index += offset;
        }
        else {
            index -= offset;
        }

        if (index < indexBounds.start() || index > indexBounds.end()) {
            return -1;
        }
    }
}

void RangeSequence::setConstantSize(qint64 constantSize, qint64 valueCount)
{
    m_constantSize = constantSize;
    if (constantSize > 0) {
        m_valueCount = valueCount;
        m_size = valueCount / constantSize + ((valueCount % constantSize == 0) ? 0 : 1);
        m_maxSize = constantSize;
    }
    else {
        m_size = 0;
        m_valueCount = 0;
        m_maxSize = 0;
    }
}

void RangeSequence::appendRange(qint64 rangeSize)
{
    if (m_constantSize > 0) {
        m_constantSize = -1;
        m_size = 0;
        m_valueCount = 0;
    }

    writeRange(m_size, m_valueCount + rangeSize - 1);
    m_valueCount += rangeSize;
    m_size++;

    m_maxSize = qMax(m_maxSize, rangeSize);
}

void RangeSequence::writeRange(qint64 i, qint64 end)
{
    QMutexLocker lock(&m_cacheMutex);

    qint64 *rangeLoc = getLocation(i);
    *rangeLoc = end;

    m_dirtyCache = true;
}

Range RangeSequence::readRange(qint64 i) const
{
    QMutexLocker lock(&m_cacheMutex);
    if (i == 0) {
        return Range(0, *getLocation(i));
    }
    Range r = Range(*getLocation(i-1) + 1, *getLocation(i));
    return r;
}

qint64 *RangeSequence::getLocation(qint64 i) const
{
    qint64 cacheIdx = i / CACHE_CHUNK_64_SIZE;

    if (cacheIdx >= m_dataCacheBlockCount) {
        resizeCache(cacheIdx + 1000);
    }

    if (!m_dataCaches[cacheIdx]) {
        if (m_recentCacheAccess.size() >= MAX_ACTIVE_CACHE_CHUNKS) {
            qint64 removeIdx = m_recentCacheAccess.dequeue();

            if (m_dirtyCache) {
                qint64 byteLoc = removeIdx * CACHE_CHUNK_BYTE_SIZE;
                m_dataFile.seek(byteLoc);
                qint64 chunkSize = qMin((m_valueCount * 8) - byteLoc, CACHE_CHUNK_BYTE_SIZE);
                m_dataFile.write(reinterpret_cast<char*>(m_dataCaches[removeIdx]), chunkSize);
            }

            delete[] m_dataCaches[removeIdx];
            m_dataCaches[removeIdx] = nullptr;
        }

        qint64* rangeBuffer = new qint64[CACHE_CHUNK_BYTE_SIZE];
        m_dataFile.seek(cacheIdx * CACHE_CHUNK_BYTE_SIZE);
        m_dataFile.read(reinterpret_cast<char*>(rangeBuffer), CACHE_CHUNK_BYTE_SIZE);
        m_dataCaches[cacheIdx] = rangeBuffer;
        m_recentCacheAccess.enqueue(cacheIdx);
    }

    return m_dataCaches[cacheIdx] + (i % CACHE_CHUNK_64_SIZE);
}

void RangeSequence::syncCacheWithFile() const
{
    if (!m_dirtyCache) {
        return;
    }

    for (qint64 idx : m_recentCacheAccess) {
        if (!m_dataCaches[idx]) {
            continue;
        }

        qint64 byteLoc = idx * CACHE_CHUNK_BYTE_SIZE;
        m_dataFile.seek(byteLoc);
        qint64 chunkSize = qMin((m_valueCount * 8) - byteLoc, CACHE_CHUNK_BYTE_SIZE);
        m_dataFile.write(reinterpret_cast<char*>(m_dataCaches[idx]), chunkSize);
    }
    m_dirtyCache = false;
}

void RangeSequence::resizeCache(int blockCount) const
{
    if (!m_dataFile.isOpen()) {
        m_dataFile.open();
    }
    syncCacheWithFile();
    clearCache();
    m_dataCacheBlockCount = blockCount;
    m_dataCaches = new qint64*[m_dataCacheBlockCount];
    for (int i = 0; i < m_dataCacheBlockCount; i++) {
        m_dataCaches[i] = nullptr;
    }
    m_dataFile.resize(m_dataCacheBlockCount * CACHE_CHUNK_BYTE_SIZE);
}

void RangeSequence::clearCache() const
{
    if (!m_dataCaches) {
        return;
    }

    for (qint64 idx : m_recentCacheAccess) {
        if (!m_dataCaches[idx]) {
            continue;
        }

        delete[] m_dataCaches[idx];
    }
    delete[] m_dataCaches;

    m_dataCaches = nullptr;
}

qint64 RangeSequence::getValueCount() const
{
    return m_valueCount;
}
