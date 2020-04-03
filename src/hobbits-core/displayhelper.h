#ifndef DISPLAYHELPER_H
#define DISPLAYHELPER_H

#include <QImage>
#include "bitcontainer.h"

namespace DisplayHelper
{
    QImage HOBBITSCORESHARED_EXPORT getBitRasterImage(QSharedPointer<const BitContainer> bits, qint64 x, qint64 y, int w, int h);
    QImage HOBBITSCORESHARED_EXPORT getByteRasterImage(QSharedPointer<const BitContainer> bits, qint64 x, qint64 y, int w, int h);
    QPixmap HOBBITSCORESHARED_EXPORT bitRasterThumbnail(QSharedPointer<const BitContainer> bits);

    QImage HOBBITSCORESHARED_EXPORT getBitRasterImage(const BitContainer * const bits, qint64 x, qint64 y, int w, int h);
    QImage HOBBITSCORESHARED_EXPORT getByteRasterImage(const BitContainer * const bits, qint64 x, qint64 y, int w, int h);
    QPixmap HOBBITSCORESHARED_EXPORT bitRasterThumbnail(const BitContainer * const bits);

};

#endif // DISPLAYHELPER_H
