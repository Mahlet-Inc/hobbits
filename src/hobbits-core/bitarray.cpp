#include "bitarray.h"

#include <QRegularExpression>
#include <QSharedPointer>
#include <QString>

static char BIT_MASKS[8] = {
    -128, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01
};

static char INVERSE_BIT_MASKS[8] = {
    0x7f, -65, -33, -17, -9, -5, -3, -2
};

static quint64 QUINT64_BIT_MASKS[64] = {
    0x8000000000000000, 0x4000000000000000, 0x2000000000000000, 0x1000000000000000,
    0x0800000000000000, 0x0400000000000000, 0x0200000000000000, 0x0100000000000000,
    0x0080000000000000, 0x0040000000000000, 0x0020000000000000, 0x0010000000000000,
    0x0008000000000000, 0x0004000000000000, 0x0002000000000000, 0x0001000000000000,
    0x0000800000000000, 0x0000400000000000, 0x0000200000000000, 0x0000100000000000,
    0x0000080000000000, 0x0000040000000000, 0x0000020000000000, 0x0000010000000000,
    0x0000008000000000, 0x0000004000000000, 0x0000002000000000, 0x0000001000000000,
    0x0000000800000000, 0x0000000400000000, 0x0000000200000000, 0x0000000100000000,
    0x0000000080000000, 0x0000000040000000, 0x0000000020000000, 0x0000000010000000,
    0x0000000008000000, 0x0000000004000000, 0x0000000002000000, 0x0000000001000000,
    0x0000000000800000, 0x0000000000400000, 0x0000000000200000, 0x0000000000100000,
    0x0000000000080000, 0x0000000000040000, 0x0000000000020000, 0x0000000000010000,
    0x0000000000008000, 0x0000000000004000, 0x0000000000002000, 0x0000000000001000,
    0x0000000000000800, 0x0000000000000400, 0x0000000000000200, 0x0000000000000100,
    0x0000000000000080, 0x0000000000000040, 0x0000000000000020, 0x0000000000000010,
    0x0000000000000008, 0x0000000000000004, 0x0000000000000002, 0x0000000000000001
};

#define CACHE_CHUNK_BYTE_SIZE (10 * 1000 * 1000)
#define CACHE_CHUNK_BIT_SIZE (CACHE_CHUNK_BYTE_SIZE * 8)
#define MAX_ACTIVE_CACHE_CHUNKS 5

BitArray::BitArray() :
    m_dataFile("bitarray"),
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
    QTemporaryFile *dataReader = const_cast<QTemporaryFile*>(&m_dataFile);
    dataReader->seek(0);
    return dataReader;
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
        qint64 cacheCount = sizeInBits() / CACHE_CHUNK_BIT_SIZE + (sizeInBits() % CACHE_CHUNK_BIT_SIZE ? 1 : 0);
        m_dataCaches = new char*[cacheCount];
        for (int i = 0; i < cacheCount; i++) {
            m_dataCaches[i] = nullptr;
        }
    }
}

void BitArray::deleteCache()
{
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


quint64 BitArray::getWordValue(qint64 bitOffset, int wordBitSize) const
{
    quint64 word = 0;
    for (qint64 i = 0; i < wordBitSize; i++) {
        if (this->at(bitOffset + i)) {
            word += 1 << (wordBitSize - i - 1);
        }
    }
    return word;
}

bool BitArray::loadCacheAt(qint64 i) const
{
    qint64 cacheIdx = i / CACHE_CHUNK_BIT_SIZE;
    int index = int(i - cacheIdx * CACHE_CHUNK_BIT_SIZE);
    if (m_dataCaches[cacheIdx]) {
        return m_dataCaches[cacheIdx][index / 8] & BIT_MASKS[index % 8];
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

    return byteBuffer[index / 8] & BIT_MASKS[index % 8];
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

void BitArray::syncCacheToFile() const
{
    if (m_dirtyCache) {
        for (qint64 cacheIdx : m_recentCacheAccess) {
            QTemporaryFile *noConstFile = const_cast<QTemporaryFile*>(&m_dataFile);
            noConstFile->seek(cacheIdx * CACHE_CHUNK_BYTE_SIZE);
            qint64 cacheChunkByteLength =
                qMin(qint64(CACHE_CHUNK_BYTE_SIZE), sizeInBytes() - (cacheIdx * CACHE_CHUNK_BYTE_SIZE));
            noConstFile->write(m_dataCaches[cacheIdx], cacheChunkByteLength);
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
    QTemporaryFile *noConstFile = const_cast<QTemporaryFile*>(&m_dataFile);
    if (!noConstFile->seek(byteOffset)) {
        return 0;
    }

    return noConstFile->read(data, maxBytes);
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
