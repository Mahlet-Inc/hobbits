#include "displayhelper.h"
#include "settingsmanager.h"
#include <QPixmap>
#include <QFont>
#include <QFontMetrics>
#include <QPainter>
#include <QtMath>
#include "displayinterface.h"

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
    QColor trueColor = SettingsManager::getUiSetting(SettingsManager::ONE_COLOR_KEY).value<QColor>();
    QColor falseColor = SettingsManager::getUiSetting(SettingsManager::ZERO_COLOR_KEY).value<QColor>();
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

    QColor c = SettingsManager::getUiSetting(SettingsManager::BYTE_HUE_SAT_KEY).value<QColor>();
    int hue = c.hue();
    int saturation = c.saturation();
    for (int i = 0; i < h; i++) {
        if (i + frameOffset >= bits->frameCount()) {
            break;
        }
        Frame frame = bits->frameAt(int(i + frameOffset));

        for (int ii = 0; ii < w * 8; ii += 8) {
            if (ii + bitOffset + 8 > frame.size()) {
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

QSize DisplayHelper::textSize(QFont font, QString text)
{
    QFontMetrics fontMetrics = QFontMetrics(font);
    return QSize(fontMetrics.horizontalAdvance(text), fontMetrics.height());
}

QSize DisplayHelper::textSize(QFont font, qint64 maxValue)
{
    return textSize(font, QString("%1").arg(maxValue));
}

QFont DisplayHelper::monoFont(int pointSize)
{
    QFont font("Roboto Mono");
    font.setPointSize(pointSize);
    font.setStyleStrategy(QFont::ForceIntegerMetrics);
    return font;
}

QColor DisplayHelper::headerBackgroundColor()
{
    return QColor("#2f2f2f");
}

QColor DisplayHelper::headerForegroundColor()
{
    return QColor("#999999");
}

QList<RangeHighlight> DisplayHelper::getHighlightSpots(QList<RangeHighlight> highlights, int &highlightMinIndex, Frame frame)
{
    QList<RangeHighlight> spots;

    unsigned int intersection = Frame::Before;
    int highlightIndex = highlightMinIndex;
    while (intersection & (Frame::Before | Frame::Overlapping)) {
        if (highlightIndex >= highlights.size()) {
            break;
        }
        RangeHighlight highlight = highlights.at(highlightIndex);
        intersection = frame.compare(highlight.range());

        if (intersection & Frame::Overlapping) {
            RangeHighlight overlap(highlight.category(), highlight.label(), frame.getOverlap(highlight.range()), highlight.color(), highlight.children());
            spots.append(overlap);
        }
        else if (intersection & Frame::After) {
            break;
        }

        if ((!(intersection & Frame::After)) && highlightMinIndex == highlightIndex) {
            highlightMinIndex++;
        }
        highlightIndex++;
    }
    return spots;
}

void DisplayHelper::drawHighlights(
        QSharedPointer<DisplayHandle> handle,
        QPainter *painter,
        QSizeF elementSize,
        QSize destSize,
        qint64 bitOffset,
        qint64 frameOffset,
        int colGroupSize,
        int colGroupMargin)
{
    if (handle.isNull() || handle->currentContainer().isNull()) {
        return;
    }
    if (handle->frameOffset() < 0 || handle->bitOffset() < 0) {
        return;
    }

    painter->setPen(Qt::transparent);
    for (QString category: handle->currentContainer()->info()->highlightCategories()) {
        drawHighlightRects(handle->currentContainer(),
                           painter,
                           category,
                           elementSize.width(),
                           elementSize.height(),
                           frameOffset,
                           bitOffset,
                           destSize.width(),
                           destSize.height(),
                           colGroupSize,
                           colGroupMargin);
    }
}

QVector<QRectF> DisplayHelper::drawHighlightRects(
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
        int colGroupMargin)
{
    if (container.isNull()) {
        return QVector<QRectF>();
    }

    QVector<QRectF> rects;
    if (container->info()->highlights(category).size() > 0) {
        int lastHighlight = 0;
        int rowOffset = -1;
        for (qint64 i = frameOffset; i < container->frameCount(); i++) {
            rowOffset++;
            if (rowOffset >= rowCount) {
                break;
            }
            Frame frame = container->frameAt(i);
            Frame displayFrame =
                Frame(
                        container->bits(),
                        frame.start() + bitOffset,
                        qMin(frame.end(), frame.start() + bitOffset + colCount - 1));
            QList<RangeHighlight> spots = getHighlightSpots(
                    container->info()->highlights(category),
                    lastHighlight,
                    displayFrame);

            while(!spots.isEmpty()) {
                RangeHighlight spot = spots.takeFirst();
                if (!spot.children().isEmpty()) {
                    painter->setOpacity(0.35);
                    int minIndex = 0;
                    spots.append(getHighlightSpots(spot.children(), minIndex, displayFrame));
                }
                else {
                    painter->setOpacity(1);
                }
                qint64 displayStart = (spot.range().start() - displayFrame.start());
                double hx = getGroupedOffset(displayStart, colWidth, colGroupSize, bitOffset, colGroupMargin);
                double hy = (i - frameOffset) * rowHeight;
                qint64 displayEnd = (spot.range().end() - displayFrame.start());
                double hw =
                    getGroupedOffset(displayEnd, colWidth, colGroupSize, bitOffset, colGroupMargin) + colWidth - hx;
                double hh = rowHeight;
                painter->setBrush(QColor::fromRgba(spot.color()));
                painter->drawRect(QRectF(hx, hy, hw, hh));
            }
        }
    }
    return rects;
}

QPoint DisplayHelper::getOffset(qint64 bitOffset, int x, int y, int xSize, int ySize, int xGrouping, int bitsPerX)
{
    int xDiff = x / xSize;
    int yDiff = y / ySize;

    if (xGrouping > 1) {
        int groupOffset = bitOffset % xGrouping;
        int xGroups = (xDiff + groupOffset) / (xGrouping + 1);
        xDiff -= xGroups;
    }
    xDiff *= bitsPerX;

    return QPoint(xDiff, yDiff);
}

double DisplayHelper::getGroupedOffset(qint64 idx, double width, int groupSize, qint64 offset, int groupMargin)
{
    if (groupSize > 1) {
        qint64 groupOffset = offset % groupSize;
        qint64 groups = (idx + groupOffset) / groupSize;
        return width * double((groups * groupMargin) + idx);
    }
    else {
        return width * idx;
    }
}



QImage DisplayHelper::drawHeadersFull(QSize viewportSize,
                                      QPoint offset,
                                      QSharedPointer<DisplayHandle> handle,
                                      QSizeF bitSize,
                                      int columnGrouping,
                                      int groupMargin)
{
    if (offset.x() == 0 && offset.y() == 0) {
        return QImage();
    }

    QImage headers(viewportSize, QImage::Format_ARGB32);
    headers.fill(Qt::transparent);
    QPainter painter(&headers);
    painter.fillRect(0, 0, offset.x(), offset.y(), DisplayHelper::headerBackgroundColor());

    painter.translate(offset);
    DisplayHelper::drawFramesHeader(&painter,
                                    QSize(offset.x(), viewportSize.height() - offset.y()),
                                    handle,
                                    bitSize.height());

    DisplayHelper::drawFramesHeader(&painter,
                                    QSize(viewportSize.width() - offset.x(), offset.y()),
                                    handle,
                                    bitSize.width(),
                                    Qt::Horizontal,
                                    columnGrouping,
                                    groupMargin);

    return headers;
}

void DisplayHelper::drawFramesHeader(QPainter *painter,
                                     QSize headerSize,
                                     QSharedPointer<DisplayHandle> displayHandle,
                                     double frameHeight,
                                     int orientation,
                                     int grouping,
                                     int groupMargin)
{
    painter->save();

    auto tickColor = headerForegroundColor();
    tickColor.setAlphaF(0.75);

    int textAlign = Qt::AlignRight;
    qint64 offset = displayHandle->frameOffset();
    qint64 maxOffset = displayHandle->currentContainer()->frameCount();
    int xOrient = -1;
    qint64 highlightOffset = displayHandle->frameOffsetHover();
    if (orientation == Qt::Horizontal) {
        painter->rotate(-90);
        xOrient = 0;
        headerSize.transpose();
        textAlign = Qt::AlignLeft;
        offset = displayHandle->bitOffset();
        maxOffset = displayHandle->currentContainer()->maxFrameWidth();
        highlightOffset = displayHandle->bitOffsetHover();
    }


    QFont font = DisplayHelper::monoFont(10);
    QSize fontSize = DisplayHelper::textSize(font, "0");
    int framesPerMarker = qMax(1, qRound(double(fontSize.height()) / frameHeight));

    int margin = fontSize.width();
    int tickSize = qMax(1, margin / 2);
    int textSize = headerSize.width() - (margin * 2);
    int yOffset = -1 * qRound((fontSize.height() - frameHeight) / 2.0);

    painter->setPen(DisplayHelper::headerForegroundColor());
    painter->setFont(font);
    painter->fillRect(xOrient * headerSize.width(), 0, headerSize.width(), headerSize.height(), DisplayHelper::headerBackgroundColor());
    int framesHeight = qRound(double(headerSize.height()) / frameHeight);
    for (int i = 0; i <= framesHeight; i += framesPerMarker) {
        if (i + offset >= maxOffset) {
            break;
        }

        int y = qRound(i * frameHeight);
        if (grouping) {
            y = int(getGroupedOffset(i, frameHeight, grouping, offset, groupMargin));
        }

        if (framesPerMarker > 1) {
            painter->fillRect(
                    xOrient * tickSize,
                    y,
                    tickSize,
                    qCeil(frameHeight),
                    tickColor);
        }

        painter->drawText(
                margin + (xOrient * (textSize + 2 * margin)),
                y + yOffset,
                textSize,
                fontSize.height(),
                textAlign,
                QString("%1").arg(i + offset));
    }

    if (highlightOffset >= 0) {
        qint64 screenOffset = highlightOffset - offset;
        int y = qRound(screenOffset * frameHeight);
        if (grouping) {
            y = int(getGroupedOffset(screenOffset, frameHeight, grouping, offset, groupMargin));
        }
        font.setBold(true);
        painter->setFont(font);
        painter->setPen(highlightForegroundColor());
        QRect backRect(
                    0,
                    y + yOffset,
                    headerSize.width() + (xOrient * headerSize.width() * 2),
                    fontSize.height());
        painter->fillRect(backRect, highlightBackgroundColor());

        painter->fillRect(
                xOrient * tickSize,
                y,
                tickSize,
                qCeil(frameHeight),
                highlightForegroundColor());

        QRect textRect(
                    margin + (xOrient * (textSize + 2 * margin)),
                    y + yOffset,
                    textSize,
                    fontSize.height());
        painter->drawText(
                textRect,
                textAlign,
                QString("%1").arg(highlightOffset));
    }

    painter->restore();
}

void DisplayHelper::drawHoverBox(QPainter *painter,
                                 QRect targetRect,
                                 QPoint hoverPoint,
                                 QStringList textLines)
{
    painter->save();

    painter->translate(targetRect.topLeft());
    hoverPoint -= targetRect.topLeft();

    QFont font = DisplayHelper::monoFont(10);
    QSize fontSize = DisplayHelper::textSize(font, "0");

    int maxLineLength = 1;
    for (QString line : textLines) {
        if (line.size() > maxLineLength) {
            maxLineLength = line.size();
        }
    }

    int pad = 4;
    int textHeight = fontSize.height() + 3;
    int boxWidth = fontSize.width() * maxLineLength + pad*textLines.length();
    int boxHeight = textHeight * textLines.size() + pad*2;

    QRect box;
    if (hoverPoint.x() > targetRect.width() / 2) {
       box.setX(hoverPoint.x() - pad - boxWidth);
    }
    else {
       box.setX(hoverPoint.x() + pad);
    }
    if (hoverPoint.y() > targetRect.height() / 2) {
       box.setY(hoverPoint.y() - pad - boxHeight);
    }
    else {
       box.setY(hoverPoint.y() + pad);
    }
    box.setWidth(boxWidth);
    box.setHeight(boxHeight);

    painter->fillRect(box, QColor(0x00, 0x00, 0x00, 0x99));

    painter->setFont(font);
    painter->setPen(QColor(0xff, 0xff, 0xff));
    for (int i = 0; i < textLines.size(); i++) {
        painter->drawText(box.x() + pad, box.y() + pad + textHeight * i, box.width(), textHeight, Qt::AlignLeft, textLines.at(i));
    }

    painter->restore();
}

QColor DisplayHelper::highlightForegroundColor()
{
    return QColor("#EEEEEE");
}

QColor DisplayHelper::highlightBackgroundColor()
{
    return QColor("#1c1c1c");
}

void DisplayHelper::setRenderRange(DisplayInterface *display, QSharedPointer<DisplayHandle> handle, int visibleFrames)
{
    if (handle->currentContainer()->frameCount() > handle->frameOffset() && handle->frameOffset() >= 0 && visibleFrames > 0) {
        qint64 start = handle->currentContainer()->frameAt(handle->frameOffset()).start();
        qint64 lastFrame = qMin(handle->frameOffset() + visibleFrames, handle->currentContainer()->frameCount() - 1);
        qint64 end = handle->currentContainer()->frameAt(lastFrame).end();
        handle->setRenderedRange(display, Range(start, end));
    }
    else {
        handle->setRenderedRange(display, Range());
    }
}

QImage DisplayHelper::drawTextRasterFull(QSize viewportSize,
                                         QPoint offset,
                                         DisplayInterface * display,
                                         QSharedPointer<DisplayHandle> handle,
                                         const Parameters &parameters,
                                         int bitsPerChar,
                                         std::function<QString (const Frame &, qint64 &)> textConverter)
{
    if (handle.isNull() || handle->currentContainer().isNull() || !display->parameterDelegate()->validate(parameters).isEmpty()) {
        return QImage();
    }

    int fontSize = parameters.value("font_size").toInt();
    int columnGrouping = parameters.value("column_grouping").toInt();
    QSize textSectionSize(viewportSize.width() - offset.x(), viewportSize.height() - offset.y());

    QColor textBg = QColor("#1c1c1c");
    QColor textFg = QColor("#eeeeee");
    QImage textImage(viewportSize, QImage::Format_ARGB32);
    textImage.fill(textBg);
    QPainter painter(&textImage);
    painter.translate(offset);
    painter.setPen(textFg);

    int rows = drawTextRaster(&painter, textSectionSize, handle, bitsPerChar, columnGrouping, fontSize, textConverter);

    DisplayHelper::setRenderRange(display, handle, rows);

    return textImage;
}

int DisplayHelper::drawTextRaster(QPainter *painter,
                                   QSize textSectionSize,
                                   QSharedPointer<DisplayHandle> handle,
                                  int bitsPerChar,
                                   int columnGrouping,
                                   int fontSize,
                                   std::function<QString (const Frame &, qint64 &)> textConverter)
{
    QFont font = DisplayHelper::monoFont(fontSize);
    QSize textSize = DisplayHelper::textSize(font, "0");
    int rowH = textRowHeight(textSize.height());

    double charWidth = textSize.width();
    if (columnGrouping > 1) {
        charWidth *= double(columnGrouping + 1) / double(columnGrouping);
    }
    int charsPerRow = qCeil(double(textSectionSize.width()) / charWidth);
    int rows = textSectionSize.height() / rowH + 1;

    painter->setFont(font);

    for (int row = 0; row < rows; row++) {
        qint64 frameOffset = row + handle->frameOffset();
        if (frameOffset >= handle->currentContainer()->frameCount()) {
            break;
        }
        Frame f = handle->currentContainer()->frameAt(frameOffset);
        QString frameString = "";
        qint64 bitOffset = (handle->bitOffset() / bitsPerChar) * bitsPerChar;
        for (int col = 0; col < charsPerRow; col++) {
            if (bitOffset >= f.size()) {
                break;
            }

            if (columnGrouping > 1 && col > 0 && bitOffset % (columnGrouping * bitsPerChar) == 0) {
                frameString += " ";
            }

            frameString += textConverter(f, bitOffset);
        }
        painter->drawText(
                        0,
                        row * rowH,
                        textSectionSize.width(),
                        rowH,
                        Qt::AlignLeft,
                        frameString);
    }

    drawHighlights(
                handle,
                painter,
                QSizeF(double(textSize.width()) / double(bitsPerChar), rowH),
                textSectionSize,
                handle->bitOffset(),
                handle->frameOffset(),
                columnGrouping,
                columnGrouping > 1 ? 1 : 0);

    return rows;
}

int DisplayHelper::textRowHeight(int textHeight)
{
    return textHeight + 2;
}

void DisplayHelper::connectHoverUpdates(DisplayInterface *display,
                                        QObject *displayObject,
                                        QSharedPointer<DisplayHandle> handle,
                                        std::function<bool (QPoint &,QSize &, int &, int &)> paramSet)
{
    QObject::connect(handle.data(), &DisplayHandle::newMouseHover, displayObject, [display, handle, paramSet](DisplayInterface *hoverDisplay, QPoint hover) {
        if (display != hoverDisplay) {
            return;
        }
        if (hover.isNull()
                || handle->currentContainer().isNull()) {
            handle->setBitHover(false);
            return;
        }

        QPoint offset(0, 0);
        QSize symbolSize(1, 1);
        int xGrouping = 1;
        int bitsPerSymbol = 1;
        if (!paramSet(offset, symbolSize, xGrouping, bitsPerSymbol)) {
            handle->setBitHover(false);
            return;
        }

        DisplayHelper::sendHoverUpdate(
                    handle,
                    hover - offset,
                    symbolSize,
                    xGrouping,
                    bitsPerSymbol);
    });
}

void DisplayHelper::sendHoverUpdate(QSharedPointer<DisplayHandle> handle, QPoint hover, QSize symbolSize, int xGrouping, int bitsPerSymbol)
{
    if (handle->currentContainer().isNull()) {
        handle->setBitHover(false);
        return;
    }

    if (hover.x() < 0 || hover.y() < 0) {
        handle->setBitHover(false);
        return;
    }

    QPoint diff = getOffset(handle->bitOffset(), hover.x(), hover.y(), symbolSize.width(), symbolSize.height(), xGrouping, bitsPerSymbol);

    if (diff.x() < 0 || diff.y() < 0) {
        handle->setBitHover(false);
        return;
    }

    handle->setBitHover(true, diff.x(), diff.y());
}

