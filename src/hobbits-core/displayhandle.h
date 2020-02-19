#ifndef DISPLAYHANDLE_H
#define DISPLAYHANDLE_H

#include "bitcontainer.h"
#include "bitcontainermanager.h"
#include <QObject>

#include "hobbits-core_global.h"

class DisplayInterface;

class QScrollBar;

class HOBBITSCORESHARED_EXPORT DisplayHandle : public QObject
{
    Q_OBJECT

public:
    explicit DisplayHandle(
            QSharedPointer<BitContainerManager> bitManager,
            QScrollBar *vScroll,
            QScrollBar *hScroll,
            QObject *parent = nullptr);

    QSharedPointer<BitContainer> getContainer() const;
    int getBitOffset() const;
    int getFrameOffset() const;
    QSet<DisplayInterface*> getCurrentFocusDisplays() const;

    QScrollBar* getVScroll();
    QScrollBar* getHScroll();

signals:
    void newBitContainer();
    void newOffsets(int bitOffset, int frameOffset);
    void newBitHover(bool, int, int);
    void newFocusDisplays(QSet<DisplayInterface*>);

    void containerFramesChanged();
    void containerHighlightsChanged();

public slots:
    void setOffsets(int bitOffset, int frameOffset);
    void setBitHover(bool hovering, int bitOffset, int frameOffset);
    void setFocusDisplays(QSet<DisplayInterface*> focusDisplays);
    void bitContainerSelected();

private slots:
    void offsetChanged();

private:
    QSharedPointer<BitContainerManager> m_bitManager;
    QScrollBar *m_vScroll;
    QScrollBar *m_hScroll;
    QSet<DisplayInterface*> m_focusDisplays;
};

#endif // DISPLAYHANDLE_H
