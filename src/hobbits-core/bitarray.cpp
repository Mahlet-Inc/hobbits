#include "bitarray.h"

#include <QRegularExpression>
#include <QSharedPointer>
#include <QString>
#include <QDir>

#ifdef Q_OS_UNIX
#define bswap64(X) __builtin_bswap64((X))
#endif
#ifdef Q_OS_WIN
#include <intrin.h>
#define bswap64(X) _byteswap_uint64((X))
#endif

static char BIT_MASKS[8] = {
    -128, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01
};

static char INVERSE_BIT_MASKS[8] = {
    0x7f, -65, -33, -17, -9, -5, -3, -2
};

static quint64 BYTE_MASKS[8] = {
    0xff00000000000000, 0x00ff000000000000, 0x0000ff0000000000, 0x000000ff00000000, 0x00000000ff000000, 0x0000000000ff0000, 0x000000000000ff00, 0x00000000000000ff
};

static quint64 INVERSE_BYTE_MASKS[8] = {
    0x00000000000000ff, 0x000000000000ff00, 0x0000000000ff0000, 0x00000000ff000000, 0x000000ff00000000, 0x0000ff0000000000, 0x00ff000000000000, 0xff00000000000000
};

static quint8 IMASK_MSB_8[8] = {
    0x00, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe
};

static quint8 IMASK_LSB_8[8] = {
    0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f
};

static quint64 COPY_64_BIT_LE_MASKS[8] = {
    0x0000000000000000, 0x0000000000000080, 0x00000000000000b0, 0x00000000000000e0, 0x00000000000000f0, 0x00000000000000f8, 0x00000000000000fb, 0x00000000000000fe
};

static quint8 MASK_MSB_8[8] {
    0xff, 0x7f, 0x3f, 0x1f, 0x0f, 0x07, 0x03, 0x01
};

static quint8 MASK_LSB_8[8] {
    0xff, 0xfe, 0xfc, 0xf8, 0xf0, 0xe0, 0xc0, 0x80
};

#define CACHE_CHUNK_BYTE_SIZE (10 * 1000 * 1000)
#define CACHE_CHUNK_BIT_SIZE (CACHE_CHUNK_BYTE_SIZE * 8)
#define MAX_ACTIVE_CACHE_CHUNKS 5

BitArray::BitArray() :
    m_dataFile(QDir::temp().absoluteFilePath("bitarray")),
    m_size(0),
    m_dataCaches(nullptr)
{
    m_dataFile.open();
}

BitArray::BitArray(qint64 sizeInBits) :
    BitArray()
{
    m_size = sizeInBits;
    qint64 bytesToWrite = this->sizeInBytes();
    char *byteBuffer = new char[CACHE_CHUNK_BYTE_SIZE];
    for (int i = 0; i < CACHE_CHUNK_BYTE_SIZE; i++) {
        byteBuffer[i] = 0;
    }
    while (bytesToWrite > 0) {
        qint64 byteChunkSize = qMin(bytesToWrite, qint64(CACHE_CHUNK_BYTE_SIZE));
        qint64 bytesWritten = m_dataFile.write(byteBuffer, byteChunkSize);
        bytesToWrite -= bytesWritten;

        if (bytesToWrite > 0 && bytesWritten < 1) {
            delete[] byteBuffer;
            throw std::invalid_argument(
                    QString("Failed to initialize BitArray file of %1 bits").arg(
                            sizeInBits).toStdString());
        }
    }
    delete[] byteBuffer;

    reinitializeCache();
}

BitArray::BitArray(QByteArray bytes, qint64 sizeInBits) :
    BitArray()
{
    if (sizeInBits < 0) {
        sizeInBits = bytes.size() * 8;
    }
    if (sizeInBits > bytes.size() * 8) {
        throw std::invalid_argument(
                QString("Cannot create BitArray of size '%2' from %1 bytes").arg(bytes.size()).arg(
                        sizeInBits).toStdString());
    }
    m_size = sizeInBits;
    m_dataFile.write(bytes);

    reinitializeCache();
}

BitArray::BitArray(QIODevice *dataStream, qint64 sizeInBits) :
    BitArray()
{
    initFromIO(dataStream, sizeInBits);
}

BitArray::BitArray(const BitArray &other) :
    BitArray(other.dataReader(), other.sizeInBits())
{
}

BitArray::BitArray(const BitArray *other) :
    BitArray(other->dataReader(), other->sizeInBits())
{
}


BitArray::~BitArray()
{
    deleteCache();
}

BitArray& BitArray::operator=(const BitArray &other)
{
    initFromIO(other.dataReader(), other.sizeInBits());
    return *this;
}

QIODevice* BitArray::dataReader() const
{
    syncCacheToFile();
    m_dataFile.seek(0);
    return &m_dataFile;
}

void BitArray::initFromIO(QIODevice *dataStream, qint64 sizeInBits)
{
    if (sizeInBits < 0) {
        sizeInBits = dataStream->bytesAvailable() * 8;
    }
    m_size = sizeInBits;
    qint64 bytesToRead = this->sizeInBytes();
    char *byteBuffer = new char[CACHE_CHUNK_BYTE_SIZE];
    while (bytesToRead > 0) {
        qint64 bytesRead = dataStream->read(byteBuffer, CACHE_CHUNK_BYTE_SIZE);
        m_dataFile.write(byteBuffer, bytesRead);
        bytesToRead -= bytesRead;

        if (bytesToRead > 0 && bytesRead < 1) {
            delete[] byteBuffer;
            throw std::invalid_argument("'dataStream' provided to BitArray constructor had fewer than 'sizeInBits' bits");
        }
    }
    delete[] byteBuffer;

    reinitializeCache();
}

void BitArray::reinitializeCache()
{
    if (m_dataCaches) {
        deleteCache();
    }
    if (sizeInBits() > 0) {
        m_cacheMutex.lock();
        qint64 cacheCount = sizeInBits() / CACHE_CHUNK_BIT_SIZE + (sizeInBits() % CACHE_CHUNK_BIT_SIZE ? 1 : 0);
        m_dataCaches = new char*[cacheCount];
        for (int i = 0; i < cacheCount; i++) {
            m_dataCaches[i] = nullptr;
        }
        m_cacheMutex.unlock();
    }
}

void BitArray::deleteCache()
{
    QMutexLocker lock(&m_cacheMutex);
    while (!m_recentCacheAccess.isEmpty()) {
        delete[] m_dataCaches[m_recentCacheAccess.dequeue()];
    }
    delete[] m_dataCaches;
}

bool BitArray::at(qint64 i) const
{
    if (i < 0 || i >= m_size) {
        throw std::invalid_argument(QString("Invalid bit index '%1'").arg(i).toStdString());
    }
    qint64 cacheIdx = i / CACHE_CHUNK_BIT_SIZE;
    if (!m_dataCaches[cacheIdx]) {
        loadCacheAt(i);
    }
    int index = int(i - cacheIdx * CACHE_CHUNK_BIT_SIZE);
    return m_dataCaches[cacheIdx][index / 8] & BIT_MASKS[index % 8];
}


char BitArray::byteAt(qint64 i) const
{
    if (i < 0 || i >= sizeInBytes()) {
        throw std::invalid_argument(QString("Invalid byte index '%1'").arg(i).toStdString());
    }
    qint64 cacheIdx = i / CACHE_CHUNK_BYTE_SIZE;
    if (!m_dataCaches[cacheIdx]) {
        loadCacheAt(i*8);
    }
    int index = int(i - cacheIdx * CACHE_CHUNK_BYTE_SIZE);
    return m_dataCaches[cacheIdx][index];
}

quint64 BitArray::getWordValue(qint64 bitOffset, int wordBitSize, bool littleEndian) const
{
    quint64 word = 0;
    for (qint64 i = 0; i < wordBitSize; i++) {
        if (this->at(bitOffset + i)) {
            word += 1 << (wordBitSize - i - 1);
        }
    }
    if (littleEndian && wordBitSize % 8 == 0) {
        quint64 buffer = 0;
        int bytes = wordBitSize/8;
        for (int i = 0; i < bytes; i++) {
            buffer += (word & INVERSE_BYTE_MASKS[i]) << ((bytes-1-i*2)*8);
        }
        word = buffer;
    }
    return word;
}

qint64 BitArray::getWordValueTwosComplement(qint64 bitOffset, int wordBitSize, bool littleEndian) const
{
    quint64 uVal = getWordValue(bitOffset, wordBitSize, littleEndian);
    qint64 *val = reinterpret_cast<qint64*>(&uVal);
    if (wordBitSize == 64) {
        return *val;
    }
    qint64 signBit = 1 << (wordBitSize-1);
    if (signBit & *val) {
        auto r = (*val - signBit) - signBit;
        return r;
    }
    else {
        return *val;
    }
}

void BitArray::loadCacheAt(qint64 i) const
{
    QMutexLocker lock(&m_cacheMutex);
    qint64 cacheIdx = i / CACHE_CHUNK_BIT_SIZE;
    if (m_dataCaches[cacheIdx]) {
        return;
    }

    QQueue<qint64> *unconstRecentCacheAccess = const_cast<QQueue<qint64>*>(&m_recentCacheAccess);
    char **unconstDataCaches = const_cast<char**>(m_dataCaches);

    qint64 byteIdx = cacheIdx * CACHE_CHUNK_BYTE_SIZE;

    char *byteBuffer = new char[CACHE_CHUNK_BYTE_SIZE];
    readBytesNoSync(byteBuffer, byteIdx, CACHE_CHUNK_BYTE_SIZE);

    unconstDataCaches[cacheIdx] = byteBuffer;
    unconstRecentCacheAccess->enqueue(cacheIdx);
    if (unconstRecentCacheAccess->size() > MAX_ACTIVE_CACHE_CHUNKS) {
        qint64 removedCacheIndex = unconstRecentCacheAccess->dequeue();

        if (m_dirtyCache) {
            QTemporaryFile *noConstFile = const_cast<QTemporaryFile*>(&m_dataFile);
            noConstFile->seek(removedCacheIndex * CACHE_CHUNK_BYTE_SIZE);
            qint64 cacheChunkByteLength =
                qMin(qint64(CACHE_CHUNK_BYTE_SIZE), sizeInBits() - (removedCacheIndex * CACHE_CHUNK_BIT_SIZE));
            noConstFile->write(unconstDataCaches[removedCacheIndex], cacheChunkByteLength);
        }

        delete[] unconstDataCaches[removedCacheIndex];
        unconstDataCaches[removedCacheIndex] = nullptr;
    }
}

qint64 BitArray::sizeInBits() const
{
    return m_size;
}

qint64 BitArray::sizeInBytes() const
{
    return m_size / 8 + (m_size % 8 ? 1 : 0);
}

void BitArray::resize(qint64 sizeInBits)
{
    QMutexLocker lock(&m_mutex);
    syncCacheToFile();
    m_size = sizeInBits;
    reinitializeCache();
    m_dataFile.resize(sizeInBytes());
}

void BitArray::set(qint64 i, bool value)
{
    if (i < 0 || i >= m_size) {
        throw std::invalid_argument(QString("Invalid bit index '%1'").arg(i).toStdString());
    }
    QMutexLocker lock(&m_mutex);

    m_dirtyCache = true;

    qint64 cacheIdx = i / CACHE_CHUNK_BIT_SIZE;
    if (!m_dataCaches[cacheIdx]) {
        loadCacheAt(i);
    }
    int index = int(i - cacheIdx * CACHE_CHUNK_BIT_SIZE);
    if (value) {
        m_dataCaches[cacheIdx][index / 8] = m_dataCaches[cacheIdx][index / 8] | BIT_MASKS[index % 8];
    }
    else {
        m_dataCaches[cacheIdx][index / 8] = m_dataCaches[cacheIdx][index / 8] & INVERSE_BIT_MASKS[index % 8];
    }
}

qint64 BitArray::copyBits(qint64 bitOffset, BitArray *dest, qint64 destBitOffset, qint64 maxBits, int copyMode) const
{
    if (!dest) {
        return 0;
    }

    // figure out how many bits will actually be copied
    qint64 bitsToCopy = qMin(maxBits, sizeInBits() - bitOffset);
    if (bitsToCopy <= 0) {
        return 0;
    }

    // resize the destination array, if neccessary
    if (dest->sizeInBits() < destBitOffset + bitsToCopy) {
        dest->resize(destBitOffset + bitsToCopy);
    }

    dest->m_mutex.lock();

    qint64 bitsCopied = 0;
    while (bitsCopied < bitsToCopy) {
        qint64 srcCacheIdx = bitOffset / CACHE_CHUNK_BIT_SIZE;
        if (!m_dataCaches[srcCacheIdx]) {
            loadCacheAt(bitOffset);
        }
        // TODO: race condition if cache is dropped between load and lock. need a recursive mutex
        m_cacheMutex.lock();

        qint64 destCacheIdx = destBitOffset / CACHE_CHUNK_BIT_SIZE;
        if (!dest->m_dataCaches[destCacheIdx]) {
            dest->loadCacheAt(destBitOffset);
        }
        dest->m_cacheMutex.lock();

        int srcByteAlignment = bitOffset % 8;
        int destByteAlignment = destBitOffset % 8;

        qint64 srcCacheBitOffset = bitOffset - (srcCacheIdx * CACHE_CHUNK_BIT_SIZE);
        qint64 destCacheBitOffset = destBitOffset - (destCacheIdx * CACHE_CHUNK_BIT_SIZE);

        qint64 srcCacheByteOffset = srcCacheBitOffset / 8;
        qint64 destCacheByteOffset = destCacheBitOffset / 8;

        char* srcCache = m_dataCaches[srcCacheIdx] + srcCacheByteOffset;
        char* destCache = dest->m_dataCaches[destCacheIdx] + destCacheByteOffset;

        qint64 bitsThisRound = CACHE_CHUNK_BIT_SIZE - qMax(srcCacheBitOffset, destCacheBitOffset);
        bitsThisRound = qMin(bitsThisRound, (bitsToCopy - bitsCopied));

        // TODO: short-circuit for byte-aligned copy here

        int srcOffset = srcByteAlignment;
        int destOffset = destByteAlignment;
        int srcByteOffset = 0;
        int destByteOffset = 0;
        qint64 increment = 0;
        qint64 bitsLeft = bitsThisRound;
        while (bitsLeft > 0) {
            if (bitsLeft < 8) {
                quint8 mask8 =  IMASK_MSB_8[destByteAlignment];

                // get the source value and shift it to align with the destination
                quint8 srcVal = quint8(srcCache[srcByteOffset]);
                if (copyMode == CopyMode::Invert) {
                    srcVal = ~srcVal;
                }
                srcVal <<= srcByteAlignment;
                srcVal >>= destByteAlignment;

                // remove any bits from the byte that extend beyond the remaining bits to copy
                int destTrailMask = 8 - destByteAlignment - int(bitsLeft);
                if (destTrailMask > 0) {
                    mask8 &= IMASK_LSB_8[destTrailMask];
                    srcVal &= MASK_LSB_8[destTrailMask];
                }

                // get the target destination byte and applythe source value
                char* target = destCache + destByteOffset;
                if (copyMode == CopyMode::Copy || copyMode == CopyMode::Invert) {
                    *target &= mask8;
                    *target |= srcVal;
                }
                else if (copyMode == CopyMode::Xor) {
                    *target ^= srcVal;
                }
                else if (copyMode == CopyMode::And) {
                    *target &= srcVal;
                }
                else if (copyMode == CopyMode::Or) {
                    *target |= srcVal;
                }

                increment = 8 - qMax(destByteAlignment, srcByteAlignment);
            }
            else if (bitsLeft < 64) {
                // copy up to 8 bits
                quint8 srcVal = quint8(srcCache[srcByteOffset]);
                if (copyMode == CopyMode::Invert) {
                    srcVal = ~srcVal;
                }
                srcVal <<= srcByteAlignment;
                srcVal >>= destByteAlignment;
                char* target = destCache + destByteOffset;

                if (copyMode == CopyMode::Copy || copyMode == CopyMode::Invert) {
                    *target &= IMASK_MSB_8[destByteAlignment];
                    *target |= srcVal;
                }
                else if (copyMode == CopyMode::Xor) {
                    *target ^= srcVal;
                }
                else if (copyMode == CopyMode::And) {
                    *target &= srcVal;
                }
                else if (copyMode == CopyMode::Or) {
                    *target |= srcVal;
                }

                increment = 8 - qMax(destByteAlignment, srcByteAlignment);
            }
            else {
                // copy up to 64 bits
                quint64 srcVal = static_cast<quint64*>(static_cast<void*>(srcCache + srcByteOffset))[0];
                if (copyMode == CopyMode::Invert) {
                    srcVal = ~srcVal;
                }
                srcVal = bswap64(srcVal);
                srcVal <<= srcByteAlignment;
                srcVal >>= destByteAlignment;
                srcVal = bswap64(srcVal);
                quint64* target = static_cast<quint64*>(static_cast<void*>(destCache + destByteOffset));

                if (copyMode == CopyMode::Copy || copyMode == CopyMode::Invert) {
                    *target &= COPY_64_BIT_LE_MASKS[destByteAlignment];
                    *target |= srcVal;
                }
                else if (copyMode == CopyMode::Xor) {
                    *target ^= srcVal;
                }
                else if (copyMode == CopyMode::And) {
                    *target &= srcVal;
                }
                else if (copyMode == CopyMode::Or) {
                    *target |= srcVal;
                }

                increment = 64 - qMax(destByteAlignment, srcByteAlignment);
            }

            bitsLeft -= increment;
            srcOffset += increment;
            destOffset += increment;
            srcByteOffset = srcOffset / 8;
            srcByteAlignment = srcOffset % 8;
            destByteOffset = destOffset / 8;
            destByteAlignment = destOffset % 8;
        }

        dest->m_cacheMutex.unlock();
        m_cacheMutex.unlock();

        bitsCopied += bitsThisRound;
        bitOffset += bitsThisRound;
        destBitOffset += bitsThisRound;
    }

    dest->m_dirtyCache = true;
    dest->m_mutex.unlock();

    return bitsToCopy;
}

void BitArray::syncCacheToFile() const
{
    if (m_dirtyCache) {
        QMutexLocker cacheLock(&m_cacheMutex);
        QMutexLocker dataFileLock(&m_dataFileMutex);
        for (qint64 cacheIdx : m_recentCacheAccess) {
            m_dataFile.seek(cacheIdx * CACHE_CHUNK_BYTE_SIZE);
            qint64 cacheChunkByteLength =
                qMin(qint64(CACHE_CHUNK_BYTE_SIZE), sizeInBytes() - (cacheIdx * CACHE_CHUNK_BYTE_SIZE));
            m_dataFile.write(m_dataCaches[cacheIdx], cacheChunkByteLength);
        }
    }
}

qint64 BitArray::readBytes(char *data, qint64 byteOffset, qint64 maxBytes) const
{
    syncCacheToFile();
    return readBytesNoSync(data, byteOffset, maxBytes);
}

void BitArray::writeTo(QIODevice *outputStream) const
{
    QIODevice *reader = dataReader();
    char *byteBuffer = new char[CACHE_CHUNK_BYTE_SIZE];
    qint64 bytesToWrite = sizeInBytes();
    while (bytesToWrite > 0) {
        qint64 bytesRead = reader->read(byteBuffer, CACHE_CHUNK_BYTE_SIZE);
        outputStream->write(byteBuffer, bytesRead);
        bytesToWrite -= bytesRead;

        if (bytesToWrite > 0 && bytesRead < 1) {
            delete[] byteBuffer;
            throw std::invalid_argument("BitArray failed to provide bytes equal to its size during writeTo");
        }
    }
    delete[] byteBuffer;
}

qint64 BitArray::readBytesNoSync(char *data, qint64 byteOffset, qint64 maxBytes) const
{
    QMutexLocker lock(&m_dataFileMutex);
    if (!m_dataFile.seek(byteOffset)) {
        return 0;
    }

    return m_dataFile.read(data, maxBytes);
}

int BitArray::getPreviewSize() const
{
    return int(qMin(sizeInBits(), qint64(CACHE_CHUNK_BIT_SIZE)));
}

QByteArray BitArray::getPreviewBytes() const
{
    loadCacheAt(0);
    QByteArray preview(m_dataCaches[0], int(qMin(sizeInBytes(), qint64(CACHE_CHUNK_BYTE_SIZE))));
    return preview;
}

QSharedPointer<BitArray> BitArray::fromString(QString bitArraySpec, QStringList parseErrors)
{
    int size = 0;
    if (bitArraySpec.startsWith("0x")) {
        QRegularExpression hexMatcher("^[0-9A-F]+$", QRegularExpression::CaseInsensitiveOption);
        QRegularExpressionMatch match = hexMatcher.match(bitArraySpec.mid(2));
        if (match.hasMatch()) {
            QString evenSpec = bitArraySpec.mid(2);
            if (evenSpec.size() % 2 == 1) {
                evenSpec += "0";
            }
            QByteArray bytes = QByteArray::fromHex(evenSpec.toLocal8Bit());
            size = qMin((bitArraySpec.length() - 2) * 4, bytes.size() * 8);
            return QSharedPointer<BitArray>(new BitArray(bytes, size));
        }
        else {
            parseErrors.append(QString("Expected only hex digits in '0x'-prefixed data - got '%1'").arg(bitArraySpec));
            return QSharedPointer<BitArray>(new BitArray());
        }
    }
    else if (bitArraySpec.startsWith("0o")) {
        size = (bitArraySpec.length() - 2) * 3;
        QSharedPointer<BitArray> bits = QSharedPointer<BitArray>(new BitArray(size));
        bool parseOk;
        for (int i = 2; i < bitArraySpec.size(); i++) {
            parseOk = true;
            int val = bitArraySpec.mid(i, 1).toInt(&parseOk, 8);

            if (!parseOk) {
                parseErrors.append(
                        QString("Expected octal digit in '0o'-prefixed data - got %1").arg(
                                bitArraySpec.at(
                                        i)));
                continue;
            }

            for (int bit = 0; bit < 3; bit++) {
                bits->set((i - 2) * 3 + bit, val & BIT_MASKS[5 + bit]);
            }
        }
        return bits;
    }
    else if (bitArraySpec.startsWith("0b")) {
        size = bitArraySpec.length() - 2;
        QSharedPointer<BitArray> bits = QSharedPointer<BitArray>(new BitArray(size));
        for (int i = 2; i < bitArraySpec.size(); i++) {
            if (bitArraySpec.at(i) == '1') {
                bits->set(i - 2, true);
            }
            else if (bitArraySpec.at(i) != '0') {
                parseErrors.append(
                        QString("Expected '1' or '0' in '0b'-prefixed data - got '%1'").arg(bitArraySpec.at(i)));
            }
        }
        return bits;
    }
    else {
        QByteArray bytes = bitArraySpec.toLocal8Bit();
        size = bitArraySpec.length() * 8;
        return QSharedPointer<BitArray>(new BitArray(bytes, size));
    }
}
