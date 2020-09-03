#include "bitinfo.h"
#include <QVariant>

BitInfo::BitInfo(QObject *parent) :
    QObject(parent),
    m_rangeSequence(new RangeSequence())
{

}

BitInfo::BitInfo(QSharedPointer<const BitArray> bits, QObject *parent) :
    QObject(parent),
    m_bits(bits),
    m_rangeSequence(new RangeSequence())
{

}


QSharedPointer<BitInfo> BitInfo::copyMetadata() const
{
    auto copy = new BitInfo();
    copy->m_rangeSequence = RangeSequence::fromOther(m_rangeSequence);
    copy->m_metadata = m_metadata;
    copy->m_rangeHighlights = m_rangeHighlights;
    return QSharedPointer<BitInfo>(copy);
}

void BitInfo::setBits(QSharedPointer<const BitArray> bits)
{
    m_mutex.lock();
    m_bits = bits;
    m_mutex.unlock();
}

void BitInfo::setFrames(QSharedPointer<RangeSequence> frames)
{
    m_mutex.lock();
    m_rangeSequence = frames;
    m_mutex.unlock();
}

void BitInfo::setFramesFromInfo(QSharedPointer<BitInfo> frameSource)
{
    m_mutex.lock();
    m_rangeSequence = RangeSequence::fromOther(frameSource->m_rangeSequence);
    m_mutex.unlock();
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
    return m_rangeSequence->getMaxSize();
}

Frame BitInfo::frameAt(qint64 i) const
{
    return Frame(m_bits, m_rangeSequence->at(i));
}

qint64 BitInfo::frameCount() const
{
    return m_rangeSequence->size();
}

QSharedPointer<const RangeSequence> BitInfo::frames() const
{
    return m_rangeSequence;
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

qint64 BitInfo::frameOffsetContaining(qint64 value, Range indexBounds) const
{
    return m_rangeSequence->indexOf(value, indexBounds);
}


const QString VERSION_1 = "BitInfo v1";
QDataStream& operator<<(QDataStream& stream, const BitInfo& bitInfo)
{
    //TODO: Serialization for range sequence
    stream << VERSION_1;
    //stream << bitInfo.m_ranges;
    //stream << bitInfo.m_maxFrameWidth;
    stream << bitInfo.m_metadata;
    stream << bitInfo.m_rangeHighlights;
    return stream;
}

QDataStream& operator>>(QDataStream& stream, BitInfo& bitInfo)
{
    stream.setStatus(QDataStream::Status::ReadCorruptData);
    QString version;
    stream >> version;
    if (version == VERSION_1) {
        //stream >> bitInfo.m_ranges;
        //stream >> bitInfo.m_maxFrameWidth;
        stream >> bitInfo.m_metadata;
        stream >> bitInfo.m_rangeHighlights;
        return stream;
    }
    else {
        stream.setStatus(QDataStream::Status::ReadCorruptData);
        return stream;
    }
}
