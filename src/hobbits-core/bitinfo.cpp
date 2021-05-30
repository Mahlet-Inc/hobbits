#include "bitinfo.h"
#include <QVariant>
#include "bitcontainer.h"

BitInfo::BitInfo() :
    QObject()
{

}

QSharedPointer<BitInfo> BitInfo::create(qint64 bitLength, QSharedPointer<const BitInfo> other, bool clearFrames)
{
    auto info = new BitInfo();

    if (!other.isNull()) {
        if (other->m_frames->getValueCount() == bitLength && !clearFrames) {
            info->m_frames = RangeSequence::fromOther(other->frames());
        }
        info->m_metadata = other->m_metadata;
        info->m_rangeHighlights = other->m_rangeHighlights;
    }

    if (other.isNull() || other->m_frames->getValueCount() != bitLength || clearFrames){
        info->m_frames = RangeSequence::fromConstantSize(256, bitLength);
    }

    return QSharedPointer<BitInfo>(info);
}

QSharedPointer<BitInfo> BitInfo::copyFromContainer(QSharedPointer<const BitContainer> container, bool clearFrames)
{
    return create(container->bits()->sizeInBits(), container->info(), clearFrames);
}

void BitInfo::setFrames(QSharedPointer<const RangeSequence> frames)
{
    m_frames = RangeSequence::fromOther(frames);
}

void BitInfo::addHighlight(RangeHighlight highlight)
{
    m_mutex.lock();
    QList<RangeHighlight> newList = this->highlights(highlight.category());
    newList.append(highlight);
    std::sort(newList.begin(), newList.end());
    m_rangeHighlights.remove(highlight.category());
    m_rangeHighlights.insert(highlight.category(), newList);
    m_mutex.unlock();
    emit changed();
}

void BitInfo::addHighlights(QList<RangeHighlight> highlights)
{
    m_mutex.lock();
    // split into different categories
    QHash<QString, QList<RangeHighlight>> added;
    for (auto highlight: highlights) {
        if (!added.contains(highlight.category())) {
            added.insert(highlight.category(), {});
        }
        added[highlight.category()].append(highlight);
    }

    // sort each category in its entirety
    for (QString category: added.keys()) {
        QList<RangeHighlight> newList = added.value(category);
        newList.append(this->highlights(category));
        std::sort(newList.begin(), newList.end());
        m_rangeHighlights.remove(category);
        m_rangeHighlights.insert(category, newList);
    }
    m_mutex.unlock();
    emit changed();
}

void BitInfo::setMetadata(QString key, QVariant value)
{
    if (m_metadata.value(key) == value) {
        return;
    }
    m_mutex.lock();
    m_metadata.remove(key);
    m_metadata.insert(key, value);
    m_mutex.unlock();
    emit changed();
}

void BitInfo::clearHighlightCategory(QString category)
{
    if (m_rangeHighlights.contains(category)) {
        m_mutex.lock();
        m_rangeHighlights.remove(category);
        m_mutex.unlock();
        emit changed();
    }
}

qint64 BitInfo::maxFrameWidth() const
{
    return m_frames->getMaxSize();
}

qint64 BitInfo::frameCount() const
{
    return m_frames->size();
}

QSharedPointer<const RangeSequence> BitInfo::frames() const
{
    return m_frames;
}

QList<RangeHighlight> BitInfo::highlights(QString category) const
{
    return m_rangeHighlights.value(category);
}

QList<RangeHighlight> BitInfo::highlights(QString category, QString label) const
{
    QList<RangeHighlight> matching;
    for (auto highlight : highlights(category)) {
        if (highlight.label() == label) {
            matching.append(highlight);
        }
        for (auto child : highlight.allDescendants()) {
            if (child.label() == label) {
                matching.append(child);
            }
        }
    }
    return matching;
}

QList<QString> BitInfo::highlightCategories() const
{
    return m_rangeHighlights.keys();
}

bool BitInfo::containsHighlightCategory(QString category) const
{
    return m_rangeHighlights.contains(category);
}

QVariant BitInfo::metadata(QString key) const
{
    return m_metadata.value(key);
}

QList<QString> BitInfo::metadataKeys() const
{
    return m_metadata.keys();
}

qint64 BitInfo::frameOffsetContaining(qint64 value, Range indexBounds) const
{
    return m_frames->indexOf(value, indexBounds);
}

QSharedPointer<BitInfo> BitInfo::deserialize(QDataStream &stream)
{
    auto info = new BitInfo();
    info->m_frames = RangeSequence::deserialize(stream);
    stream >> info->m_rangeHighlights;
    stream >> info->m_metadata;

    return QSharedPointer<BitInfo>(info);
}

void BitInfo::serialize(QDataStream &stream) const
{
    m_frames->serialize(stream);
    stream << m_rangeHighlights;
    stream << m_metadata;
}
