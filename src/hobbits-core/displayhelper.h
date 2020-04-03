#ifndef DISPLAYHELPER_H
#define DISPLAYHELPER_H

#include <QImage>
#include "bitcontainer.h"

namespace DisplayHelper
{
    QImage getBitRasterImage(QSharedPointer<const BitContainer> bits, qint64 x, qint64 y, int w, int h);
    QImage getByteRasterImage(QSharedPointer<const BitContainer> bits, qint64 x, qint64 y, int w, int h);
    QPixmap bitRasterThumbnail(QSharedPointer<const BitContainer> bits);

    QImage getBitRasterImage(const BitContainer * const bits, qint64 x, qint64 y, int w, int h);
    QImage getByteRasterImage(const BitContainer * const bits, qint64 x, qint64 y, int w, int h);
    QPixmap bitRasterThumbnail(const BitContainer * const bits);

};

#endif // DISPLAYHELPER_H
