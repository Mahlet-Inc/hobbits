#ifndef BITARRAY_H
#define BITARRAY_H

#include <QByteArray>
#include <QIODevice>
#include <QMap>
#include <QQueue>
#include <QStringList>
#include <QTemporaryFile>

#include "hobbits-core_global.h"

class HOBBITSCORESHARED_EXPORT BitArray
{
public:
    BitArray();
    BitArray(qint64 sizeInBits);
    BitArray(QByteArray bytes, qint64 sizeInBits);
    BitArray(QIODevice *dataStream, qint64 sizeInBits);
    BitArray(const BitArray &other, qint64 sizeInBits);
    BitArray(const BitArray &other);

    BitArray& operator=(const BitArray &other);

    ~BitArray();

    bool at(qint64 i) const;
    qint64 sizeInBits() const;
    qint64 sizeInBytes() const;

    void resize(qint64 sizeInBits);

    void set(qint64 i, bool value);

    qint64 readBytes(char *data, qint64 byteOffset, qint64 maxBytes) const;
    void writeTo(QIODevice *outputStream) const;

    int getPreviewSize() const;
    QByteArray getPreviewBytes() const;

    static QSharedPointer<BitArray> fromString(QString bitArraySpec, QStringList parseErrors = QStringList());

private:
    qint64 readBytesNoSync(char *data, qint64 byteOffset, qint64 maxBytes) const;
    QIODevice* dataReader() const;
    void initFromIO(QIODevice *dataStream, qint64 sizeInBits);
    void reinitializeCache();
    void deleteCache();
    bool loadCacheAt(qint64 bitIndex) const;
    void syncCacheToFile() const;

    QTemporaryFile m_dataFile;
    qint64 m_size;

    QQueue<qint64> m_recentCacheAccess;
    char **m_dataCaches;
    bool m_dirtyCache;
};

inline bool operator==(const BitArray &b1, const BitArray &b2)
{
    return b1.getPreviewBytes().compare(b2.getPreviewBytes()) == 0;
}

inline uint qHash(const BitArray &key, uint seed)
{
    return qHash(key.getPreviewBytes(), seed) ^ uint(key.getPreviewSize());
}

#endif // BITARRAY_H
