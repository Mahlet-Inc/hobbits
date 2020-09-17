#include "metadatahelper.h"

MetadataHelper& MetadataHelper::instance()
{
    static MetadataHelper instance;
    return instance;
}

QString MetadataHelper::sampleFormatKey()
{
    return "sample_format";
}

QList<MetadataHelper::SampleFormat> MetadataHelper::sampleFormats()
{
    return instance().m_sampleFormats;
}

MetadataHelper::SampleFormat MetadataHelper::sampleFormat(QString id)
{
    return instance().m_sampleFormatMap.value(id);
}

MetadataHelper::MetadataHelper()
{
    m_sampleFormats = {
        {"int8", "8-bit Integer", 8, false},
        {"uint8", "8-bit Unsigned Integer", 8, false},
        {"int16_le", "16-bit Integer", 16, true},
        {"uint16_le", "16-bit Unsigned Integer", 16, true},
        {"int24_le", "24-bit Integer", 24, true},
        {"uint24_le", "24-bit Unsigned Integer", 24, true},
        {"int32_le", "32-bit Integer", 32, true},
        {"uint32_le", "32-bit Unsigned Integer", 32, true},
        {"int64_le", "64-bit Integer", 64, true},
        {"uint64_le", "64-bit Unsigned Integer", 64, true},
        {"float32_le", "32-bit IEEE 754 Float", 32, true},
        {"float64_le", "64-bit IEEE 754 Float", 64, true},
        {"int16_be", "16-bit Big Endian Integer", 16, false},
        {"uint16_be", "16-bit Big Endian Unsigned Integer", 16, false},
        {"int24_be", "24-bit Big Endian Integer", 24, false},
        {"uint24_be", "24-bit Big Endian Unsigned Integer", 24, false},
        {"int32_be", "32-bit Big Endian Integer", 32, false},
        {"uint32_be", "32-bit Big Endian Unsigned Integer", 32, false},
        {"int64_be", "64-bit Big Endian Integer", 64, false},
        {"uint64_be", "64-bit Big Endian Unsigned Integer", 64, false},
        {"float32_be", "32-bit Big Endian IEEE 754 Float", 32, false},
        {"float64_be", "64-bit Big Endian IEEE 754 Float", 64, false},
    };
    for (auto format : m_sampleFormats) {
        m_sampleFormatMap.insert(format.id, format);
    }
}
