#ifndef METADATAHELPER_H
#define METADATAHELPER_H

#include <QString>
#include <QScopedPointer>
#include <QMap>

class MetadataHelper
{
public:
    static MetadataHelper& instance();

    struct SampleFormat {
        QString id;
        QString name;
        int wordSize;
        bool littleEndian;
    };

    static QString sampleFormatKey();
    static QList<SampleFormat> sampleFormats();
    static SampleFormat sampleFormat(QString id);

private:
    MetadataHelper();

    QList<SampleFormat> m_sampleFormats;
    QMap<QString, SampleFormat> m_sampleFormatMap;
public:
    MetadataHelper(MetadataHelper const&) = delete;
    void operator=(MetadataHelper const&) = delete;
};

#endif // METADATAHELPER_H
