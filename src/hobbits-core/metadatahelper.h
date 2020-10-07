#ifndef METADATAHELPER_H
#define METADATAHELPER_H

#include <QString>
#include <QScopedPointer>
#include <QMap>
#include "hobbits-core_global.h"

/**
  * @brief The MetadataHelper class facilitates coordinated usage of BitInfo across plugins
  *
  * Since the metadata in BitInfo can be so flexible, it is important for different plugins
  * to have shared expectations about metadata values that they might encounter or add
  * during processing.
  *
  * \see BitInfo
*/
class HOBBITSCORESHARED_EXPORT MetadataHelper
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
