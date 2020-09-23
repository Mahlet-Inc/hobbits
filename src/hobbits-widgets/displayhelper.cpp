#include "displayhelper.h"
#include "settingsmanager.h"
#include <QPixmap>

QImage DisplayHelper::getBitRasterImage(QSharedPointer<const BitContainer> bits, qint64 x, qint64 y, int w, int h)
{
    return DisplayHelper::getBitRasterImage(bits.data(), x, y, w, h);
}

QImage DisplayHelper::getByteRasterImage(QSharedPointer<const BitContainer> bits, qint64 x, qint64 y, int w, int h)
{
    return DisplayHelper::getByteRasterImage(bits.data(), x, y, w, h);
}

QPixmap DisplayHelper::bitRasterThumbnail(QSharedPointer<const BitContainer> bits)
{
    return DisplayHelper::bitRasterThumbnail(bits.data());
}

QImage DisplayHelper::getBitRasterImage(const BitContainer * const bits, qint64 x, qint64 y, int w, int h)
{
    QColor trueColor = SettingsManager::getInstance().getUiSetting(SettingsData::ONE_COLOR_KEY).value<QColor>();
    QColor falseColor = SettingsManager::getInstance().getUiSetting(SettingsData::ZERO_COLOR_KEY).value<QColor>();
    QImage raster(w, h, QImage::Format_ARGB32);
    raster.fill(qRgba(0x66, 0x66, 0x66, 0x66));

    qint64 frameOffset = y;
    qint64 bitOffset = x;

    if (frameOffset < 0) {
        return raster;
    }

    for (int i = 0; i < h; i++) {
        if (i + frameOffset >= bits->frameCount()) {
            break;
        }
        Frame frame = bits->frameAt(int(i + frameOffset));

        for (int ii = 0; ii < w; ii++) {
            if (ii + bitOffset >= frame.size()) {
                break;
            }

            if (frame.at(ii + bitOffset)) {
                raster.setPixel(ii, i, trueColor.rgba());
            }
            else {
                raster.setPixel(ii, i, falseColor.rgba());
            }
        }
    }
    return raster;
}

QImage DisplayHelper::getByteRasterImage(const BitContainer * const bits, qint64 x, qint64 y, int w, int h)
{
    QImage raster(w, h, QImage::Format_ARGB32);
    raster.fill(qRgba(0x66, 0x66, 0x66, 0x66));

    qint64 frameOffset = y;
    qint64 byteOffset = x / 8;
    qint64 bitOffset = byteOffset * 8;

    if (frameOffset < 0) {
        return raster;
    }

    QColor c = SettingsManager::getInstance().getUiSetting(SettingsData::BYTE_HUE_SAT_KEY).value<QColor>();
    int hue = c.hue();
    int saturation = c.saturation();
    for (int i = 0; i < h; i++) {
        if (i + frameOffset >= bits->frameCount()) {
            break;
        }
        Frame frame = bits->frameAt(int(i + frameOffset));

        for (int ii = 0; ii < w * 8; ii += 8) {
            if (ii + bitOffset + 8 >= frame.size()) {
                break;
            }

            quint8 byteVal = 0;
            for (int bit = 0; bit < 8; bit++) {
                if (frame.at(ii + bit + bitOffset)) {
                    byteVal |= 0x80 >> bit;
                }
            }

            c.setHsl(hue, saturation, byteVal);
            raster.setPixel(ii / 8, i, c.rgba());
        }
    }
    return raster;
}

QPixmap DisplayHelper::bitRasterThumbnail(const BitContainer * const bits)
{
    return QPixmap::fromImage(DisplayHelper::getBitRasterImage(bits, 0, 0, 64, 64));
}
