#ifndef BITARRAY_H
#define BITARRAY_H

#include <QByteArray>
#include <QIODevice>
#include <QMap>
#include <QQueue>
#include <QStringList>
#include <QTemporaryFile>
#include <QMutex>

#include "hobbits-core_global.h"

class HOBBITSCORESHARED_EXPORT BitArray
{
public:
    enum CopyMode {
        Copy = 0,
        Invert = 1,
        Xor = 2,
        And = 3,
        Or = 4
    };

    BitArray();
    BitArray(qint64 sizeInBits);
    BitArray(QByteArray bytes, qint64 sizeInBits);
    BitArray(QIODevice *dataStream, qint64 sizeInBits);
    BitArray(const BitArray &other, qint64 sizeInBits);
    BitArray(const BitArray &other);
    BitArray(const BitArray *other);

    BitArray& operator=(const BitArray &other);

    ~BitArray();

    bool at(qint64 i) const;
    char byteAt(qint64 i) const;
    qint64 sizeInBits() const;
    qint64 sizeInBytes() const;
    quint64 getWordValue(qint64 bitOffset, int wordBitSize, bool littleEndian = false) const;
    qint64 getWordValueTwosComplement(qint64 bitOffset, int wordBitSize, bool littleEndian = false) const;

    void resize(qint64 sizeInBits);

    void set(qint64 i, bool value);

    qint64 copyBits(qint64 bitOffset, BitArray *dest, qint64 destBitOffset, qint64 maxBits, int copyMode = CopyMode::Copy) const;

    qint64 readBytes(char *data, qint64 byteOffset, qint64 maxBytes) const;
    QByteArray readBytes(qint64 byteOffset, qint64 maxBytes) const;
    void writeTo(QIODevice *outputStream) const;

    static QSharedPointer<BitArray> fromString(QString bitArraySpec, QStringList parseErrors = QStringList());

private:
    qint64 readBytesNoSync(char *data, qint64 byteOffset, qint64 maxBytes) const;
    QByteArray readBytesNoSync(qint64 byteOffset, qint64 maxBytes) const;
    QIODevice* dataReader() const;
    void initFromIO(QIODevice *dataStream, qint64 sizeInBits);
    void reinitializeCache();
    void deleteCache();
    void loadCacheAt(qint64 bitIndex) const;
    void syncCacheToFile() const;

    mutable QTemporaryFile m_dataFile;
    qint64 m_size;

    QQueue<qint64> m_recentCacheAccess;
    char **m_dataCaches;
    bool m_dirtyCache;

    QMutex m_mutex;
    mutable QMutex m_cacheMutex;
    mutable QMutex m_dataFileMutex;
};

#endif // BITARRAY_H
