#include "bitcontainerpreview.h"
#include <QVariant>

BitContainerPreview::BitContainerPreview(QSharedPointer<BitContainer> bitContainer) :
    m_bitContainer(bitContainer)
{
    connect(bitContainer.data(), SIGNAL(changed()), this, SIGNAL(changed()));
}

QSharedPointer<BitContainerPreview> BitContainerPreview::wrap(QSharedPointer<BitContainer> bitContainer)
{
    return QSharedPointer<BitContainerPreview>(new BitContainerPreview(bitContainer));
}

QString BitContainerPreview::name() const
{
    return m_bitContainer->name();
}

QSharedPointer<const BitArray> BitContainerPreview::bits() const
{
    return m_bitContainer->bits();
}

QSharedPointer<const BitInfo> BitContainerPreview::info() const
{
    return m_bitContainer->info();
}

void BitContainerPreview::addHighlight(RangeHighlight highlight)
{
    m_bitContainer->info()->addHighlight(highlight);
}

void BitContainerPreview::addHighlights(QList<RangeHighlight> highlights)
{
    m_bitContainer->info()->addHighlights(highlights);
}

void BitContainerPreview::setMetadata(QString key, QVariant value)
{
    m_bitContainer->info()->setMetadata(key, value);
}

void BitContainerPreview::clearHighlightCategory(QString category)
{
    m_bitContainer->info()->clearHighlightCategory(category);
}
