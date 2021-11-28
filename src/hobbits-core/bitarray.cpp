#include "bitarray.h"

#include <format>
#include <stdexcept>
#include <algorithm> 

#ifdef Q_OS_UNIX
#define bswap16(X) __builtin_bswap16((X))
#define bswap32(X) __builtin_bswap32((X))
#define bswap64(X) __builtin_bswap64((X))
#endif
#ifdef Q_OS_WIN
#include <intrin.h>
#define bswap16(X) _byteswap_ushort((X))
#define bswap32(X) _byteswap_ulong((X))
#define bswap64(X) _byteswap_uint64((X))
#endif


static char BIT_MASKS[8] = {
    -128, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01
};

static char INVERSE_BIT_MASKS[8] = {
    0x7f, -65, -33, -17, -9, -5, -3, -2
};

//static uint64_t BYTE_MASKS[8] = {
//    0xff00000000000000, 0x00ff000000000000, 0x0000ff0000000000, 0x000000ff00000000, 0x00000000ff000000, 0x0000000000ff0000, 0x000000000000ff00, 0x00000000000000ff
//};

static uint64_t INVERSE_BYTE_MASKS[8] = {
    0x00000000000000ff, 0x000000000000ff00, 0x0000000000ff0000, 0x00000000ff000000, 0x000000ff00000000, 0x0000ff0000000000, 0x00ff000000000000, 0xff00000000000000
};

static uint8_t IMASK_MSB_8[8] = {
    0x00, 0x80, 0xc0, 0xe0, 0xf0, 0xf8, 0xfc, 0xfe
};

static uint8_t IMASK_LSB_8[8] = {
    0x00, 0x01, 0x03, 0x07, 0x0f, 0x1f, 0x3f, 0x7f
};

static uint64_t COPY_64_BIT_LE_MASKS[8] = {
    0x0000000000000000, 0x0000000000000080, 0x00000000000000b0, 0x00000000000000e0, 0x00000000000000f0, 0x00000000000000f8, 0x00000000000000fb, 0x00000000000000fe
};

//static uint8_t MASK_MSB_8[8] {
//    0xff, 0x7f, 0x3f, 0x1f, 0x0f, 0x07, 0x03, 0x01
//};

static uint8_t MASK_LSB_8[8] {
    0xff, 0xfe, 0xfc, 0xf8, 0xf0, 0xe0, 0xc0, 0x80
};

#define CACHE_CHUNK_BYTE_SIZE (10 * 1000 * 1000)
#define CACHE_CHUNK_BIT_SIZE (CACHE_CHUNK_BYTE_SIZE * 8)
#define MAX_ACTIVE_CACHE_CHUNKS 5

std::string random_string( size_t length )
{
    auto randchar = []() -> char
    {
        const char charset[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
        const size_t max_index = (sizeof(charset) - 1);
        return charset[ rand() % max_index ];
    };
    std::string str(length,0);
    std::generate_n( str.begin(), length, randchar );
    return str;
}

BitArray::BitArray() :
    m_size(0),
    m_dataCaches(nullptr)
{
    m_dataFilePath = fs::temp_directory_path() / ("bitarr" + random_string(8));
    while (fs::exists(m_dataFilePath)) {
        m_dataFilePath = fs::temp_directory_path() / ("bitarr" + random_string(8));
    }
    m_dataFile.open(m_dataFilePath, std::fstream::in | std::fstream::out | std::fstream::binary);
}

BitArray::BitArray(int64_t sizeInBits) :
    BitArray()
{
    m_size = sizeInBits;

    fs::resize_file(m_dataFilePath, this->sizeInBytes());

    reinitializeCache();
}

BitArray::BitArray(const char *bytes, int64_t sizeInBits) :
    BitArray()
{
    m_size = sizeInBits;
    m_dataFile.write(bytes, this->sizeInBytes());

    reinitializeCache();
}

BitArray::BitArray(fs::path filepath, int64_t sizeInBits) :
    BitArray()
{
    if (sizeInBits < 0) {
        sizeInBits = fs::file_size(filepath) * 8;
    }
    
    std::ifstream fs(filepath, std::fstream::in | std::fstream::binary);
    initFromStream(fs, sizeInBits);
    fs.close();
}

BitArray::BitArray(std::istream &dataStream, int64_t sizeInBits) :
    BitArray()
{
    initFromStream(dataStream, sizeInBits);
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
    m_dataFile.close();
    deleteCache();
}

BitArray& BitArray::operator=(const BitArray &other)
{
    initFromStream(other.dataReader(), other.sizeInBits());
    return *this;
}

std::istream& BitArray::dataReader() const
{
    syncCacheToFile();
    m_dataFile.seekg(0);
    return m_dataFile;
}

void BitArray::initFromStream(std::istream &dataStream, int64_t sizeInBits)
{
    m_size = sizeInBits;
    int64_t bytesToRead = this->sizeInBytes();
    char *byteBuffer = new char[CACHE_CHUNK_BYTE_SIZE];
    while (bytesToRead > 0) {
        int64_t actualBytes = std::min(bytesToRead, int64_t(CACHE_CHUNK_BYTE_SIZE));
        int64_t bytesRead = dataStream.readsome(byteBuffer, actualBytes);
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
        int64_t cacheCount = sizeInBits() / CACHE_CHUNK_BIT_SIZE + (sizeInBits() % CACHE_CHUNK_BIT_SIZE ? 1 : 0);
        m_dataCaches = new char*[cacheCount];
        for (int i = 0; i < cacheCount; i++) {
            m_dataCaches[i] = nullptr;
        }
        m_cacheMutex.unlock();
    }
}

void BitArray::deleteCache()
{
    std::scoped_lock lock(m_cacheMutex);
    while (m_recentCacheAccess.size() > 0) {
        delete[] m_dataCaches[m_recentCacheAccess.front()];
        m_recentCacheAccess.pop_front();
    }
    delete[] m_dataCaches;
}

bool BitArray::at(int64_t i) const
{
    if (i < 0 || i >= m_size) {
        throw std::invalid_argument(std::format("Invalid bit index '{}'", i));
    }
    CacheLoadLocker lock(i, this);
    int64_t cacheIdx = i / CACHE_CHUNK_BIT_SIZE;
    int index = int(i - cacheIdx * CACHE_CHUNK_BIT_SIZE);
    return m_dataCaches[cacheIdx][index / 8] & BIT_MASKS[index % 8];
}


char BitArray::byteAt(int64_t i) const
{
    if (i < 0 || i >= sizeInBytes()) {
        throw std::invalid_argument(std::format("Invalid byte index '{}'", i));
    }
    CacheLoadLocker lock(i * 8, this);
    int64_t cacheIdx = i / CACHE_CHUNK_BYTE_SIZE;
    int index = int(i - cacheIdx * CACHE_CHUNK_BYTE_SIZE);
    return m_dataCaches[cacheIdx][index];
}

uint64_t BitArray::parseUIntValue(int64_t bitOffset, int wordBitSize, bool littleEndian) const
{
    uint64_t word = 0;
    for (int64_t i = 0; i < wordBitSize; i++) {
        if (this->at(bitOffset + i)) {
            word += 1ull << (wordBitSize - i - 1);
        }
    }
    if (littleEndian && wordBitSize % 8 == 0) {
        uint64_t buffer = 0;
        int bytes = wordBitSize/8;
        for (int i = 0; i < bytes; i++) {
            buffer += (word & INVERSE_BYTE_MASKS[i]) << ((bytes-1-i*2)*8);
        }
        word = buffer;
    }
    return word;
}

int64_t BitArray::parseIntValue(int64_t bitOffset, int wordBitSize, bool littleEndian) const
{
    uint64_t uVal = parseUIntValue(bitOffset, wordBitSize, littleEndian);
    int64_t *val = reinterpret_cast<int64_t*>(&uVal);
    if (wordBitSize == 64) {
        return *val;
    }
    int64_t signBit = 1 << (wordBitSize-1);
    if (signBit & *val) {
        auto r = (*val - signBit) - signBit;
        return r;
    }
    else {
        return *val;
    }
}

int64_t BitArray::readInt16Samples(int16_t *data, int64_t sampleOffset, int64_t maxSamples, bool bigEndian) const
{
    return readUInt16Samples(reinterpret_cast<uint16_t*>(data), sampleOffset, maxSamples, bigEndian);
}

int64_t BitArray::readUInt16Samples(uint16_t *data, int64_t sampleOffset, int64_t maxSamples, bool bigEndian) const
{
    int64_t samples = readBytes(reinterpret_cast<char*>(data), sampleOffset * 2, maxSamples * 2);
    samples /= 2;

    if (bigEndian) {
        for (int i = 0; i < samples; i++) {
            data[i] = bswap16(data[i]);
        }
    }

    return samples;
}

int64_t BitArray::readInt24Samples(int32_t *data, int64_t sampleOffset, int64_t maxSamples, bool bigEndian) const
{
    int64_t bitOffset = sampleOffset * 24;
    int64_t samples = 0;
    while (samples < maxSamples && bitOffset + 24 < sizeInBits()) {
        data[samples] = int32_t(parseIntValue(bitOffset, 24, !bigEndian));
        bitOffset += 24;
        samples++;
    }
    return samples;
}

int64_t BitArray::readUInt24Samples(uint32_t *data, int64_t sampleOffset, int64_t maxSamples, bool bigEndian) const
{
    int64_t bitOffset = sampleOffset * 24;
    int64_t samples = 0;
    while (samples < maxSamples && bitOffset + 24 < sizeInBits()) {
        data[samples] = uint32_t(parseUIntValue(bitOffset, 24, !bigEndian));
        bitOffset += 24;
        samples++;
    }
    return samples;
}

int64_t BitArray::readInt32Samples(int32_t *data, int64_t sampleOffset, int64_t maxSamples, bool bigEndian) const
{
    return readUInt32Samples(reinterpret_cast<uint32_t*>(data), sampleOffset, maxSamples, bigEndian);
}

int64_t BitArray::readUInt32Samples(uint32_t *data, int64_t sampleOffset, int64_t maxSamples, bool bigEndian) const
{
    int64_t samples = readBytes(reinterpret_cast<char*>(data), sampleOffset * 4, maxSamples * 4);
    samples /= 4;

    if (bigEndian) {
        for (int i = 0; i < samples; i++) {
            data[i] = bswap32(data[i]);
        }
    }

    return samples;
}

int64_t BitArray::readInt64Samples(int64_t *data, int64_t sampleOffset, int64_t maxSamples, bool bigEndian) const
{
    return readUInt64Samples(reinterpret_cast<uint64_t*>(data), sampleOffset, maxSamples, bigEndian);
}

int64_t BitArray::readUInt64Samples(uint64_t *data, int64_t sampleOffset, int64_t maxSamples, bool bigEndian) const
{
    int64_t samples = readBytes(reinterpret_cast<char*>(data), sampleOffset * 8, maxSamples * 8);
    samples /= 8;

    if (bigEndian) {
        for (int i = 0; i < samples; i++) {
            data[i] = bswap64(data[i]);
        }
    }

    return samples;
}

int64_t BitArray::readFloat32Samples(float *data, int64_t sampleOffset, int64_t maxSamples, bool bigEndian) const
{
    return readUInt32Samples(reinterpret_cast<uint32_t*>(data), sampleOffset, maxSamples, bigEndian);
}

int64_t BitArray::readFloat64Samples(double *data, int64_t sampleOffset, int64_t maxSamples, bool bigEndian) const
{
    return readUInt64Samples(reinterpret_cast<uint64_t*>(data), sampleOffset, maxSamples, bigEndian);
}

void BitArray::loadCacheAt(int64_t i) const
{
    int64_t cacheIdx = i / CACHE_CHUNK_BIT_SIZE;
    if (m_dataCaches[cacheIdx]) {
        return;
    }

    int64_t byteIdx = cacheIdx * CACHE_CHUNK_BYTE_SIZE;

    char *byteBuffer = new char[CACHE_CHUNK_BYTE_SIZE];
    readBytesNoSync(byteBuffer, byteIdx, CACHE_CHUNK_BYTE_SIZE);

    m_dataCaches[cacheIdx] = byteBuffer;
    m_recentCacheAccess.push_back(cacheIdx);

    if (m_recentCacheAccess.size() > MAX_ACTIVE_CACHE_CHUNKS) {
        int64_t removedCacheIndex = m_recentCacheAccess.front();
        m_recentCacheAccess.pop_front();

        if (m_dirtyCache) {
            m_dataFile.seekp(removedCacheIndex * CACHE_CHUNK_BYTE_SIZE);
            int64_t cacheChunkByteLength = std::min(int64_t(CACHE_CHUNK_BYTE_SIZE), sizeInBits() - (removedCacheIndex * CACHE_CHUNK_BIT_SIZE));
            m_dataFile.write(m_dataCaches[removedCacheIndex], cacheChunkByteLength);
        }

        delete[] m_dataCaches[removedCacheIndex];
        m_dataCaches[removedCacheIndex] = nullptr;
    }
}

int64_t BitArray::sizeInBits() const
{
    return m_size;
}

int64_t BitArray::sizeInBytes() const
{
    return m_size / 8 + (m_size % 8 ? 1 : 0);
}

void BitArray::resize(int64_t sizeInBits)
{
    std::scoped_lock lock(m_mutex);
    syncCacheToFile();
    m_size = sizeInBits;
    reinitializeCache();
    fs::resize_file(m_dataFilePath, sizeInBytes());
}

void BitArray::set(int64_t i, bool value)
{
    if (i < 0 || i >= m_size) {
        throw std::invalid_argument(QString("Invalid bit index '%1'").arg(i).toStdString());
    }
    std::scoped_lock lock(m_mutex);

    m_dirtyCache = true;

    CacheLoadLocker cacheLock(i, this);
    int64_t cacheIdx = i / CACHE_CHUNK_BIT_SIZE;
    int index = int(i - cacheIdx * CACHE_CHUNK_BIT_SIZE);
    if (value) {
        m_dataCaches[cacheIdx][index / 8] = m_dataCaches[cacheIdx][index / 8] | BIT_MASKS[index % 8];
    }
    else {
        m_dataCaches[cacheIdx][index / 8] = m_dataCaches[cacheIdx][index / 8] & INVERSE_BIT_MASKS[index % 8];
    }
}

void BitArray::setBytes(int64_t byteOffset, const char *src, int64_t srcByteOffset, int64_t length)
{
    if (sizeInBytes() < byteOffset + length) {
        resize((byteOffset + length) * 8);
    }

    std::scoped_lock lock(m_mutex);
    m_dirtyCache = true;

    int64_t bytesToCopy = length;
    while (bytesToCopy > 0) {
        CacheLoadLocker lock(byteOffset * 8, this);
        int64_t cacheIdx = byteOffset / CACHE_CHUNK_BYTE_SIZE;

        int64_t byteIndex = byteOffset - (cacheIdx * CACHE_CHUNK_BYTE_SIZE);

        int64_t chunkLength =  std::min(CACHE_CHUNK_BYTE_SIZE - byteIndex, bytesToCopy);

        memcpy(&m_dataCaches[cacheIdx][byteIndex], src + srcByteOffset, chunkLength);

        bytesToCopy -= chunkLength;
        byteOffset += chunkLength;
        srcByteOffset += chunkLength;
    }
}

int64_t BitArray::copyBits(int64_t bitOffset, BitArray *dest, int64_t destBitOffset, int64_t maxBits, int copyMode) const
{
    if (!dest) {
        return 0;
    }

    // figure out how many bits will actually be copied
    int64_t bitsToCopy = std::min(maxBits, sizeInBits() - bitOffset);
    if (bitsToCopy <= 0) {
        return 0;
    }

    // resize the destination array, if neccessary
    if (dest->sizeInBits() < destBitOffset + bitsToCopy) {
        dest->resize(destBitOffset + bitsToCopy);
    }

    dest->m_mutex.lock();

    int64_t bitsCopied = 0;
    while (bitsCopied < bitsToCopy) {
        CacheLoadLocker srcLock(bitOffset, this);
        CacheLoadLocker destLock(destBitOffset, dest);

        int64_t srcCacheIdx = bitOffset / CACHE_CHUNK_BIT_SIZE;
        int64_t destCacheIdx = destBitOffset / CACHE_CHUNK_BIT_SIZE;

        int srcByteAlignment = bitOffset % 8;
        int destByteAlignment = destBitOffset % 8;

        int64_t srcCacheBitOffset = bitOffset - (srcCacheIdx * CACHE_CHUNK_BIT_SIZE);
        int64_t destCacheBitOffset = destBitOffset - (destCacheIdx * CACHE_CHUNK_BIT_SIZE);

        int64_t srcCacheByteOffset = srcCacheBitOffset / 8;
        int64_t destCacheByteOffset = destCacheBitOffset / 8;

        char* srcCache = m_dataCaches[srcCacheIdx] + srcCacheByteOffset;
        char* destCache = dest->m_dataCaches[destCacheIdx] + destCacheByteOffset;

        int64_t bitsThisRound = CACHE_CHUNK_BIT_SIZE - std::max(srcCacheBitOffset, destCacheBitOffset);
        bitsThisRound = std::min(bitsThisRound, (bitsToCopy - bitsCopied));

        // TODO: short-circuit for byte-aligned copy here

        int srcOffset = srcByteAlignment;
        int destOffset = destByteAlignment;
        int srcByteOffset = 0;
        int destByteOffset = 0;
        int64_t increment = 0;
        int64_t bitsLeft = bitsThisRound;
        while (bitsLeft > 0) {
            if (bitsLeft < 8) {
                uint8_t mask8 =  IMASK_MSB_8[destByteAlignment];

                // get the source value and shift it to align with the destination
                uint8_t srcVal = uint8_t(srcCache[srcByteOffset]);
                if (copyMode == CopyMode::Invert) {
                    srcVal = ~srcVal;
                }
                srcVal <<= srcByteAlignment;
                srcVal >>= destByteAlignment;

                // remove any bits from the byte that extend beyond the remaining bits to copy
                int destTrailMask = 8 - destByteAlignment - int(bitsLeft);
                if (destTrailMask > 0) {
                    mask8 |= IMASK_LSB_8[destTrailMask];
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

                increment = 8 - std::max(destByteAlignment, srcByteAlignment);
            }
            else if (bitsLeft < 64) {
                // copy up to 8 bits
                uint8_t srcVal = uint8_t(srcCache[srcByteOffset]);
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

                increment = 8 - std::max(destByteAlignment, srcByteAlignment);
            }
            else {
                // copy up to 64 bits
                uint64_t srcVal = static_cast<uint64_t*>(static_cast<void*>(srcCache + srcByteOffset))[0];
                if (copyMode == CopyMode::Invert) {
                    srcVal = ~srcVal;
                }
                srcVal = bswap64(srcVal);
                srcVal <<= srcByteAlignment;
                srcVal >>= destByteAlignment;
                srcVal = bswap64(srcVal);
                uint64_t* target = static_cast<uint64_t*>(static_cast<void*>(destCache + destByteOffset));

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

                increment = 64 - std::max(destByteAlignment, srcByteAlignment);
            }

            bitsLeft -= increment;
            srcOffset += increment;
            destOffset += increment;
            srcByteOffset = srcOffset / 8;
            srcByteAlignment = srcOffset % 8;
            destByteOffset = destOffset / 8;
            destByteAlignment = destOffset % 8;
        }

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
        std::scoped_lock lock(m_cacheMutex, m_dataFileMutex);
        for (int64_t cacheIdx : m_recentCacheAccess) {
            m_dataFile.seekp(cacheIdx * CACHE_CHUNK_BYTE_SIZE);
            int64_t cacheChunkByteLength =
                std::min(int64_t(CACHE_CHUNK_BYTE_SIZE), sizeInBytes() - (cacheIdx * CACHE_CHUNK_BYTE_SIZE));
            m_dataFile.write(m_dataCaches[cacheIdx], cacheChunkByteLength);
        }
    }
}

int64_t BitArray::readBytes(char *data, int64_t byteOffset, int64_t maxBytes) const
{
    syncCacheToFile();
    return readBytesNoSync(data, byteOffset, maxBytes);
}

BitArray* BitArray::deserialize(std::istream &stream)
{
    int64_t sizeInBits;
    stream.read(reinterpret_cast<char*>(&sizeInBits), 8);
    if (sizeInBits < 0) {
        return nullptr;
    }
    auto bitArray = new BitArray();
    bitArray->initFromStream(stream, sizeInBits);
    return bitArray;
}

void BitArray::serialize(std::ostream &stream) const
{
    stream.write(reinterpret_cast<const char*>(&m_size), 8);
    writeTo(stream);
}

void BitArray::writeTo(std::ostream &outputStream) const
{
    dataReader();
    char *byteBuffer = new char[CACHE_CHUNK_BYTE_SIZE];
    int64_t bytesToWrite = sizeInBytes();
    while (bytesToWrite > 0) {
        int64_t actualBytes = std::min(bytesToWrite, int64_t(CACHE_CHUNK_BYTE_SIZE));
        int64_t bytesRead = m_dataFile.readsome(byteBuffer, actualBytes);
        outputStream.write(byteBuffer, bytesRead);
        bytesToWrite -= bytesRead;

        if (bytesToWrite > 0 && bytesRead < 1) {
            delete[] byteBuffer;
            throw std::invalid_argument("BitArray failed to provide bytes equal to its size during writeTo");
        }
    }
    delete[] byteBuffer;
}

int64_t BitArray::readBytesNoSync(char *data, int64_t byteOffset, int64_t maxBytes) const
{
    if (byteOffset >= this->sizeInBytes()) {
        return 0;
    }
    std::scoped_lock lock(m_dataFileMutex);
    m_dataFile.seekg(byteOffset);
    return m_dataFile.readsome(data, maxBytes);
}

QSharedPointer<BitArray> BitArray::fromString(QString bitArraySpec, QStringList *parseErrors)
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
            size = std::min((bitArraySpec.length() - 2) * 4, bytes.size() * 8);
            return QSharedPointer<BitArray>(new BitArray(bytes, size));
        }
        else {
            if (parseErrors != nullptr) {
                parseErrors->append(QString("Expected only hex digits in '0x'-prefixed data - got '%1'").arg(bitArraySpec));
            }
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
                if (parseErrors != nullptr) {
                    parseErrors->append(
                        QString("Expected octal digit in '0o'-prefixed data - got %1").arg(
                                bitArraySpec.at(
                                        i)));
                }
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
                if (parseErrors != nullptr) {
                    parseErrors->append(
                        QString("Expected '1' or '0' in '0b'-prefixed data - got '%1'").arg(bitArraySpec.at(i)));
                }
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

BitArray::CacheLoadLocker::CacheLoadLocker(int64_t bitIndex, const BitArray *bitArray) :
    m_locker(bitArray->m_cacheMutex) {
    int64_t srcCacheIdx = bitIndex / CACHE_CHUNK_BIT_SIZE;
    if (!bitArray->m_dataCaches[srcCacheIdx]) {
        bitArray->loadCacheAt(bitIndex);
    }
}
