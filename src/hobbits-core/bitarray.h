#ifndef BITARRAY_H
#define BITARRAY_H

#include <cstdint>
#include <iostream>
#include <mutex>
#include <deque>
#include <filesystem>
#include <fstream>

namespace fs = std::filesystem;

/**
  * @brief The BitArray class simplifies the use of byte-packed bitwise data
  *
  * \see BitContainer
*/
class BitArray
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
    BitArray(int64_t sizeInBits);
    BitArray(const char *bytes, int64_t sizeInBits = -1);
    BitArray(fs::path filepath, int64_t sizeInBits = -1);
    BitArray(std::istream &dataStream, int64_t sizeInBits);
    BitArray(const BitArray &other, int64_t sizeInBits);
    BitArray(const BitArray &other);
    BitArray(const BitArray *other);

    BitArray& operator=(const BitArray &other);

    ~BitArray();

    bool at(int64_t i) const;
    char byteAt(int64_t i) const;
    int64_t sizeInBits() const;
    int64_t sizeInBytes() const;
    uint64_t parseUIntValue(int64_t bitOffset, int wordBitSize, bool littleEndian = false) const;
    int64_t parseIntValue(int64_t bitOffset, int wordBitSize, bool littleEndian = false) const;
    int64_t readInt16Samples(int16_t *data, int64_t sampleOffset, int64_t maxSamples, bool bigEndian = false) const;
    int64_t readUInt16Samples(uint16_t *data, int64_t sampleOffset, int64_t maxSamples, bool bigEndian = false) const;
    int64_t readInt24Samples(int32_t *data, int64_t sampleOffset, int64_t maxSamples, bool bigEndian = false) const;
    int64_t readUInt24Samples(uint32_t *data, int64_t sampleOffset, int64_t maxSamples, bool bigEndian = false) const;
    int64_t readInt32Samples(int32_t *data, int64_t sampleOffset, int64_t maxSamples, bool bigEndian = false) const;
    int64_t readUInt32Samples(uint32_t *data, int64_t sampleOffset, int64_t maxSamples, bool bigEndian = false) const;
    int64_t readInt64Samples(int64_t *data, int64_t sampleOffset, int64_t maxSamples, bool bigEndian = false) const;
    int64_t readUInt64Samples(uint64_t *data, int64_t sampleOffset, int64_t maxSamples, bool bigEndian = false) const;
    int64_t readFloat32Samples(float *data, int64_t sampleOffset, int64_t maxSamples, bool bigEndian = false) const;
    int64_t readFloat64Samples(double *data, int64_t sampleOffset, int64_t maxSamples, bool bigEndian = false) const;

    void resize(int64_t sizeInBits);

    void set(int64_t i, bool value);
    void setBytes(int64_t byteOffset, const char *src, int64_t srcByteOffset, int64_t length);

    int64_t copyBits(int64_t bitOffset, BitArray *dest, int64_t destBitOffset, int64_t maxBits, int copyMode = CopyMode::Copy) const;

    int64_t readBytes(char *data, int64_t byteOffset, int64_t maxBytes) const;
    void writeTo(std::ostream &outputStream) const;

    static BitArray* deserialize(std::istream &stream);
    void serialize(std::ostream &stream) const;

    static std::shared_ptr<BitArray> fromString(std::string bitArraySpec, std::vector<std::string> *parseErrors = nullptr);

private:

    class CacheLoadLocker {
    public:
        CacheLoadLocker(int64_t bitIndex, const BitArray* bitArray);
    private:
        std::scoped_lock<std::mutex> m_locker;
    };

    int64_t readBytesNoSync(char *data, int64_t byteOffset, int64_t maxBytes) const;
    std::istream& dataReader() const;
    void initFromStream(std::istream &dataStream, int64_t sizeInBits);
    void reinitializeCache();
    void deleteCache();
    void loadCacheAt(int64_t bitIndex) const;
    void syncCacheToFile() const;

    mutable std::fstream m_dataFile;
    fs::path m_dataFilePath;
    
    int64_t m_size;

    mutable std::deque<int64_t> m_recentCacheAccess;
    mutable char **m_dataCaches;
    bool m_dirtyCache;

    std::mutex m_mutex;
    mutable std::mutex m_cacheMutex;
    mutable std::mutex m_dataFileMutex;
};

#endif // BITARRAY_H
