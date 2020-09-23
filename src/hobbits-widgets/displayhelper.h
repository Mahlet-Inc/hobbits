#ifndef DISPLAYHELPER_H
#define DISPLAYHELPER_H

#include <QImage>
#include "bitcontainer.h"
#include "hobbits-widgets_global.h"

namespace DisplayHelper
{
    QImage HOBBITSWIDGETSSHARED_EXPORT getBitRasterImage(QSharedPointer<const BitContainer> bits, qint64 x, qint64 y, int w, int h);
    QImage HOBBITSWIDGETSSHARED_EXPORT getByteRasterImage(QSharedPointer<const BitContainer> bits, qint64 x, qint64 y, int w, int h);
    QPixmap HOBBITSWIDGETSSHARED_EXPORT bitRasterThumbnail(QSharedPointer<const BitContainer> bits);

    QImage HOBBITSWIDGETSSHARED_EXPORT getBitRasterImage(const BitContainer * const bits, qint64 x, qint64 y, int w, int h);
    QImage HOBBITSWIDGETSSHARED_EXPORT getByteRasterImage(const BitContainer * const bits, qint64 x, qint64 y, int w, int h);
    QPixmap HOBBITSWIDGETSSHARED_EXPORT bitRasterThumbnail(const BitContainer * const bits);

};

#endif // DISPLAYHELPER_H
