#ifndef DISPLAYHELPER_H
#define DISPLAYHELPER_H

#include <QImage>
#include "bitcontainer.h"
#include "hobbits-widgets_global.h"
#include "displayhandle.h"

namespace DisplayHelper
{
    QImage HOBBITSWIDGETSSHARED_EXPORT getBitRasterImage(QSharedPointer<const BitContainer> bits, qint64 x, qint64 y, int w, int h);
    QImage HOBBITSWIDGETSSHARED_EXPORT getByteRasterImage(QSharedPointer<const BitContainer> bits, qint64 x, qint64 y, int w, int h);
    QPixmap HOBBITSWIDGETSSHARED_EXPORT bitRasterThumbnail(QSharedPointer<const BitContainer> bits);

    QImage HOBBITSWIDGETSSHARED_EXPORT getBitRasterImage(const BitContainer * const bits, qint64 x, qint64 y, int w, int h);
    QImage HOBBITSWIDGETSSHARED_EXPORT getByteRasterImage(const BitContainer * const bits, qint64 x, qint64 y, int w, int h);
    QPixmap HOBBITSWIDGETSSHARED_EXPORT bitRasterThumbnail(const BitContainer * const bits);

    QFont HOBBITSWIDGETSSHARED_EXPORT monoFont(int pointSize);
    QColor HOBBITSWIDGETSSHARED_EXPORT headerBackgroundColor();
    QColor HOBBITSWIDGETSSHARED_EXPORT headerForegroundColor();
    QColor HOBBITSWIDGETSSHARED_EXPORT highlightForegroundColor();
    QColor HOBBITSWIDGETSSHARED_EXPORT highlightBackgroundColor();

    QPoint HOBBITSWIDGETSSHARED_EXPORT getOffset(qint64 bitOffset, int x, int y, int xSize, int ySize, int xGrouping, int bitsPerX);
    double HOBBITSWIDGETSSHARED_EXPORT getGroupedOffset(qint64 idx, double width, int groupSize, qint64 offset, int groupMargin);

    QList<RangeHighlight> HOBBITSWIDGETSSHARED_EXPORT getHighlightSpots(QList<RangeHighlight> highlights, int &highlightMinIndex, Frame frame);
    void HOBBITSWIDGETSSHARED_EXPORT drawHighlights(
            QSharedPointer<DisplayHandle> handle,
            QPainter *painter,
            QSizeF elementSize,
            QSize destSize,
            qint64 bitOffset,
            qint64 frameOffset,
            int colGroupSize,
            int colGroupMargin = 1);
    QVector<QRectF> HOBBITSWIDGETSSHARED_EXPORT drawHighlightRects(
            QSharedPointer<const BitContainer> container,
            QPainter *painter,
            QString category,
            double colWidth,
            double rowHeight,
            qint64 frameOffset,
            qint64 bitOffset,
            int colCount,
            int rowCount,
            int colGroupSize,
            int colGroupMargin = 1);

    QSize HOBBITSWIDGETSSHARED_EXPORT textSize(QFont font, QString text);
    QSize HOBBITSWIDGETSSHARED_EXPORT textSize(QFont font, qint64 maxValue);

    void HOBBITSWIDGETSSHARED_EXPORT drawFramesHeader(QPainter *painter,
                                                      QSize viewportSize,
                                                      QSharedPointer<DisplayHandle> displayHandle,
                                                      double frameHeight,
                                                      int orientation = Qt::Vertical);


    void HOBBITSWIDGETSSHARED_EXPORT drawHoverBox(QPainter *painter,
                                                  QRect targetRect,
                                                  QPoint hoverPoint,
                                                  QStringList textLines);


    void HOBBITSWIDGETSSHARED_EXPORT setRenderRange(DisplayInterface *display,
                                                    QSharedPointer<DisplayHandle> displayHandle,
                                                    int visibleFrames);
};

#endif // DISPLAYHELPER_H
