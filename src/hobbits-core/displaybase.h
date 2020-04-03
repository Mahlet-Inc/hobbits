#ifndef DISPLAYBASE_H
#define DISPLAYBASE_H

#include "bitcontainer.h"
#include "displayhandle.h"
#include <QScrollBar>
#include <QWidget>

#include "hobbits-core_global.h"

class HOBBITSCORESHARED_EXPORT DisplayBase : public QWidget
{
    Q_OBJECT

public:
    explicit DisplayBase(
            QSharedPointer<DisplayHandle> displayHandle,
            DisplayInterface *pluginRef,
            QWidget *parent = nullptr);

protected:
    QSharedPointer<DisplayHandle> m_displayHandle;
    DisplayInterface *m_pluginRef;

    QPoint m_lastHover;

    void wheelEvent(QWheelEvent *event) override;
    void leaveEvent(QEvent *event) override;

    QList<RangeHighlight> getHighlightSpots(QList<RangeHighlight> highlights, int &highlightMinIndex, Frame frame);

    QPoint getOffset(int x, int y, int xSize, int ySize, int xGrouping, int bitsPerX);
    void sendHoverUpdate(QMouseEvent *event, int xSize, int ySize, int xGrouping, int bitsPerX, QPoint offset);

    double getGroupedOffset(qint64 idx, double width, int groupSize, int offset, int groupMargin);
    void drawHighlights(
            QPainter *painter,
            double colWidth,
            double rowHeight,
            int frameOffset,
            int bitOffset,
            int colCount,
            int rowCount,
            int colGroupSize,
            int colGroupMargin = 1);
    QVector<QRectF> drawHighlightRects(
            QPainter *painter,
            QString category,
            double colWidth,
            double rowHeight,
            int frameOffset,
            int bitOffset,
            int colCount,
            int rowCount,
            int colGroupSize,
            int colGroupMargin = 1);

signals:
    void newOffsets(int bitOffset, int frameOffset);
    void bitHover(bool, int, int);

public slots:
    void bitContainerChanged();
    void checkFocus(QSet<DisplayInterface*>);

protected slots:
    virtual void adjustScrollbars();
    virtual void showContextMenu(const QPoint &point);

};

#endif // DISPLAYBASE_H
