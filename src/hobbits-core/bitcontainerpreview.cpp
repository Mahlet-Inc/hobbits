#include "bitcontainerpreview.h"
#include <QVariant>

BitContainerPreview::BitContainerPreview(QSharedPointer<BitContainer> bitContainer) :
    m_bitContainer(bitContainer)
{
}

QString BitContainerPreview::getName() const
{
    return m_bitContainer->name();
}

QSharedPointer<const BitArray> BitContainerPreview::bits() const
{
    return m_bitContainer->bits();
}

QSharedPointer<const BitInfo> BitContainerPreview::bitInfo() const
{
    return m_bitContainer->bitInfo();
}

void BitContainerPreview::addHighlight(RangeHighlight highlight)
{
    m_bitContainer->addHighlight(highlight);
}

void BitContainerPreview::addHighlights(QList<RangeHighlight> highlights)
{
    m_bitContainer->addHighlights(highlights);
}

void BitContainerPreview::setMetadata(QString key, QVariant value)
{
    m_bitContainer->setMetadata(key, value);
}

void BitContainerPreview::clearHighlightCategory(QString category)
{
    m_bitContainer->clearHighlightCategory(category);
}
