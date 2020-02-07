#include "bitcontainerpreview.h"

BitContainerPreview::BitContainerPreview(QSharedPointer<BitContainer> bitContainer) :
    m_bitContainer(bitContainer)
{
    connect(
            m_bitContainer.data(),
            SIGNAL(highlightsChanged(BitContainer*)),
            this,
            SIGNAL(highlightsChanged(BitContainer*)));
    connect(m_bitContainer.data(), SIGNAL(framesChanged(BitContainer*)), this, SIGNAL(framesChanged(BitContainer*)));
    connect(m_bitContainer.data(), SIGNAL(changed(BitContainer*)), this, SIGNAL(changed(BitContainer*)));
    connect(m_bitContainer.data(), SIGNAL(focusRequested(int,int)), this, SIGNAL(focusRequested(int,int)));
}

QList<Frame> BitContainerPreview::getFrames() const
{
    return m_bitContainer->getFrames();
}

int BitContainerPreview::getMaxFrameWidth() const
{
    return m_bitContainer->getMaxFrameWidth();
}

QList<Range> BitContainerPreview::getHighlights(QString type) const
{
    return m_bitContainer->getHighlights(type);
}

void BitContainerPreview::setHighlights(QString type, QList<Range> highlights)
{
    m_bitContainer->setHighlights(type, highlights);
}

QStringList BitContainerPreview::getMetadata(QString type) const
{
    return m_bitContainer->getMetadata(type);
}

void BitContainerPreview::setMetadata(QString type, QStringList metadata)
{
    m_bitContainer->setMetadata(type, metadata);
}

QString BitContainerPreview::getName() const
{
    return m_bitContainer->getName();
}

void BitContainerPreview::setName(QString name)
{
    m_bitContainer->setName(name);
}

QPixmap BitContainerPreview::getThumbnail() const
{
    return m_bitContainer->getThumbnail();
}

QImage BitContainerPreview::getRasterImage(int x, int y, int w, int h) const
{
    return m_bitContainer->getRasterImage(x, y, w, h);
}

QSharedPointer<const BitArray> BitContainerPreview::getBaseBits() const
{
    return m_bitContainer->getBaseBits();
}

int BitContainerPreview::getFrameOffsetContaining(Range target) const
{
    return m_bitContainer->getFrameOffsetContaining(target);
}

int BitContainerPreview::getLastFrameOffsetFocus() const
{
    return m_bitContainer->getLastFrameOffsetFocus();
}

int BitContainerPreview::getLastBitOffsetFocus() const
{
    return m_bitContainer->getLastBitOffsetFocus();
}

void BitContainerPreview::requestFocus(int bitOffset, int frameOffset)
{
    m_bitContainer->requestFocus(bitOffset, frameOffset);
}
