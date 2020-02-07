#include "displayhandle.h"
#include <QScrollBar>

DisplayHandle::DisplayHandle(
    QSharedPointer<BitContainerManager> bitManager,
    QScrollBar *vScroll,
    QScrollBar *hScroll,
    QObject *parent) :
    QObject(parent),
    m_bitManager(bitManager),
    m_vScroll(vScroll),
    m_hScroll(hScroll)
{
    connect(
            m_bitManager.data(),
            SIGNAL(currSelectionChanged(const QItemSelection&,const QItemSelection&)),
            this,
            SLOT(bitContainerSelected()));
    connect(m_vScroll, SIGNAL(valueChanged(int)), this, SLOT(offsetChanged()));
    connect(m_hScroll, SIGNAL(valueChanged(int)), this, SLOT(offsetChanged()));
}

QSharedPointer<BitContainer> DisplayHandle::getContainer() const
{
    return m_bitManager->getCurrentContainer();
}

int DisplayHandle::getBitOffset() const
{
    return m_hScroll->value();
}

int DisplayHandle::getFrameOffset() const
{
    return m_vScroll->value();
}

QSet<DisplayInterface*> DisplayHandle::getCurrentFocusDisplays() const
{
    return m_focusDisplays;
}

QScrollBar* DisplayHandle::getVScroll()
{
    return m_vScroll;
}

QScrollBar* DisplayHandle::getHScroll()
{
    return m_hScroll;
}

void DisplayHandle::setOffsets(int bitOffset, int frameOffset)
{
    m_vScroll->setValue(frameOffset);
    m_hScroll->setValue(bitOffset);
    offsetChanged();
}

void DisplayHandle::offsetChanged()
{
    emit newOffsets(getBitOffset(), getFrameOffset());
}

void DisplayHandle::setBitHover(bool hovering, int bitOffset, int frameOffset)
{
    emit newBitHover(hovering, bitOffset, frameOffset);
}

void DisplayHandle::setFocusDisplays(QSet<DisplayInterface*> focusDisplays)
{
    m_focusDisplays = focusDisplays;
    emit newFocusDisplays(m_focusDisplays);
}

void DisplayHandle::bitContainerSelected()
{
    disconnect(this, SIGNAL(containerFramesChanged()));
    disconnect(this, SIGNAL(containerHighlightsChanged()));

    emit newBitContainer();

    if (!getContainer().isNull()) {
        connect(getContainer().data(), SIGNAL(framesChanged(BitContainer*)), this, SIGNAL(containerFramesChanged()));
        connect(
                getContainer().data(),
                SIGNAL(highlightsChanged(BitContainer*)),
                this,
                SIGNAL(containerHighlightsChanged()));

        emit containerFramesChanged();
        emit containerHighlightsChanged();
    }
}
