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

/**
  * @brief The BitArray class simplifies the use of byte-packed bitwise data
  *
  * \see BitContainer
*/
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
    BitArray(QByteArray bytes, qint64 sizeInBits = -1);
    BitArray(QIODevice *dataStream, qint64 sizeInBits = -1);
    BitArray(const BitArray &other, qint64 sizeInBits);
    BitArray(const BitArray &other);
    BitArray(const BitArray *other);

    BitArray& operator=(const BitArray &other);

    ~BitArray();

    bool at(qint64 i) const;
    char byteAt(qint64 i) const;
    qint64 sizeInBits() const;
    qint64 sizeInBytes() const;
    quint64 parseUIntValue(qint64 bitOffset, int wordBitSize, bool littleEndian = false) const;
    qint64 parseIntValue(qint64 bitOffset, int wordBitSize, bool littleEndian = false) const;
    qint64 readInt16Samples(qint16 *data, qint64 sampleOffset, qint64 maxSamples, bool bigEndian = false) const;
    qint64 readUInt16Samples(quint16 *data, qint64 sampleOffset, qint64 maxSamples, bool bigEndian = false) const;
    qint64 readInt24Samples(qint32 *data, qint64 sampleOffset, qint64 maxSamples, bool bigEndian = false) const;
    qint64 readUInt24Samples(quint32 *data, qint64 sampleOffset, qint64 maxSamples, bool bigEndian = false) const;
    qint64 readInt32Samples(qint32 *data, qint64 sampleOffset, qint64 maxSamples, bool bigEndian = false) const;
    qint64 readUInt32Samples(quint32 *data, qint64 sampleOffset, qint64 maxSamples, bool bigEndian = false) const;
    qint64 readInt64Samples(qint64 *data, qint64 sampleOffset, qint64 maxSamples, bool bigEndian = false) const;
    qint64 readUInt64Samples(quint64 *data, qint64 sampleOffset, qint64 maxSamples, bool bigEndian = false) const;
    qint64 readFloat32Samples(float *data, qint64 sampleOffset, qint64 maxSamples, bool bigEndian = false) const;
    qint64 readFloat64Samples(double *data, qint64 sampleOffset, qint64 maxSamples, bool bigEndian = false) const;

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
