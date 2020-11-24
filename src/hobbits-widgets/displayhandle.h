#ifndef DISPLAYHANDLE_H
#define DISPLAYHANDLE_H

#include "bitcontainer.h"
#include "bitcontainermanager.h"
#include <QObject>
#include <QSet>
#include <QPoint>
#include <QAbstractSlider>

#include "hobbits-widgets_global.h"

class DisplayInterface;
class DisplayWidget;
class QScrollBar;

class HOBBITSWIDGETSSHARED_EXPORT DisplayHandle : public QObject
{
    Q_OBJECT

public:
    explicit DisplayHandle(QSharedPointer<BitContainerManager> bitManager);

    QSharedPointer<BitContainer> currentContainer() const;
    qint64 bitOffset() const;
    qint64 frameOffset() const;
    QSet<DisplayWidget*> activeDisplays() const;

    void setFrameOffsetControl(QAbstractSlider *frameOffsetControl);
    void setBitOffsetControl(QAbstractSlider *bitOffsetControl);
    QAbstractSlider* frameOffsetControl();
    QAbstractSlider* bitOffsetControl();

    qint64 frameOffsetHover() const;
    qint64 bitOffsetHover() const;

    QPoint mouseHover(DisplayInterface* display) const;

    Range renderedRange(DisplayInterface* display) const;

Q_SIGNALS:
    void newBitOffset(qint64 bitOffset);
    void newFrameOffset(qint64 frameOffset);
    void newOffsets(qint64 bitOffset, qint64 frameOffset);
    void newMouseHover(DisplayInterface*, QPoint);
    void newBitHover(qint64, qint64);
    void newStatus(QString);
    void newActiveDisplays(QSet<DisplayWidget*>);
    void containerChanged();
    void renderedRangeChanged(Range);

    void overlayRedrawRequested(DisplayInterface* display);
    void fullRedrawRequested(DisplayInterface* display);

public Q_SLOTS:
    void setBitOffset(qint64 bitOffset);
    void setFrameOffset(qint64 frameOffset);
    void setOffsets(qint64 bitOffset, qint64 frameOffset);
    void setMouseHover(DisplayInterface* display, QPoint mouseHover);
    void setBitHover(bool hovering, qint64 bitOffset = 0, qint64 frameOffset = 0);
    void setStatus(QString status);
    void setActiveDisplays(QSet<DisplayWidget*> activeDisplays);
    void setRenderedRange(DisplayInterface* display, Range range);

    void requestOverlayRedraw(DisplayInterface* display=nullptr);
    void requestFullRedraw(DisplayInterface* display=nullptr);

private Q_SLOTS:
    void offsetControlUpdate();
    void configureControls();
    void managerUpdate();
    void containerUpdate();

private:
    QSharedPointer<BitContainerManager> m_bitManager;
    QSet<DisplayWidget*> m_activeDisplays;

    qint64 m_bitOffset;
    qint64 m_frameOffset;

    QAbstractSlider *m_bitOffsetControl;
    QAbstractSlider *m_frameOffsetControl;

    QHash<DisplayInterface*, QPoint> m_mouseHoverMap;
    qint64 m_bitOffsetHover;
    qint64 m_frameOffsetHover;

    QHash<DisplayInterface*, Range> m_renderedRangeMap;
};

#endif // DISPLAYHANDLE_H
