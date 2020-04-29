#include "bitinfo.h"
#include <QVariant>

BitInfo::BitInfo(QObject *parent) :
    QObject(parent),
    m_maxFrameWidth(0)
{

}

BitInfo::BitInfo(QSharedPointer<const BitArray> bits, QObject *parent) :
    QObject(parent),
    m_bits(bits),
    m_maxFrameWidth(0)
{

}


QSharedPointer<BitInfo> BitInfo::copyMetadata() const
{
    auto copy = new BitInfo();
    copy->m_ranges = m_ranges;
    copy->m_metadata = m_metadata;
    copy->m_rangeHighlights = m_rangeHighlights;
    return QSharedPointer<BitInfo>(copy);
}

void BitInfo::setBits(QSharedPointer<const BitArray> bits)
{
    m_mutex.lock();
    m_bits = bits;
    m_mutex.unlock();
    initFrames();
}

void BitInfo::setFrames(QVector<Range> frames)
{
    m_mutex.lock();
    m_ranges = frames;
    m_mutex.unlock();
    initFrames();
}

qint64 BitInfo::maxFrameWidth() const
{
    return m_maxFrameWidth;
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

QVector<Frame> BitInfo::frames() const
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

void BitInfo::initFrames()
{
    m_mutex.lock();
    m_maxFrameWidth = 0;
    m_frames.clear();

    if (m_bits.isNull()) {
        for (Range range: m_ranges) {
            m_maxFrameWidth = qMax(range.size(), m_maxFrameWidth);
        }
    }
    else {
        for (Range range: m_ranges) {
            if (range.start() >= m_bits->sizeInBits()) {
                break;
            }
            if (range.end() >= m_bits->sizeInBits()) {
                range = Range(range.start(), m_bits->sizeInBits() - 1);
            }
            m_frames.push_back(Frame(m_bits, range));
            m_maxFrameWidth = qMax(range.size(), m_maxFrameWidth);
        }
    }
    m_mutex.unlock();
    emit changed();
}

int BitInfo::frameOffsetContaining(Range target, Range frameRange) const
{
    if (frames().isEmpty()) {
        return -1;
    }
    if (!frameRange.isNull()) {
        if (frameRange.end() > frames().size() || frameRange.start() < 0) {
            return -1;
        }
    }
    else {
        frameRange = Range(0, frames().size() - 1);
    }

    if (frameRange.size() < 50) {
        for (qint64 i = frameRange.start(); i <= frameRange.end(); i++) {
            unsigned int compare = target.compare(frames().at(int(i)));
            if (compare & Frame::Overlapping) {
                return int(i);
            }
        }
        return -1;
    }

    qint64 offset = frameRange.start() + frameRange.size() / 2;
    qint64 index = offset;
    int countdown = 7;
    qint64 lowNo = frameRange.start() - 1;
    while (true) {
        unsigned int compare = target.compare(frames().at(int(index)));

        if (compare & Frame::Overlapping) {
            if (index > lowNo + 1) {
                int earlierIndex = frameOffsetContaining(target, Range(lowNo + 1, index - 1));
                if (earlierIndex >= 0) {
                    return earlierIndex;
                }
            }
            return int(index);
        }

        if (countdown < 1) {
            return -1;
        }

        offset /= 2;
        if (offset < 1) {
            offset = 1;
            countdown--;
        }

        if (compare & Frame::Before) {
            index += offset;
        }
        else if (compare & Frame::After) {
            index -= offset;
        }

        if (index < frameRange.start() || index > frameRange.end()) {
            return -1;
        }
    }
}


const QString VERSION_1 = "BitInfo v1";
QDataStream& operator<<(QDataStream& stream, const BitInfo& bitInfo)
{
    stream << VERSION_1;
    stream << bitInfo.m_ranges;
    stream << bitInfo.m_maxFrameWidth;
    stream << bitInfo.m_metadata;
    stream << bitInfo.m_rangeHighlights;
    return stream;
}

QDataStream& operator>>(QDataStream& stream, BitInfo& bitInfo)
{
    QString version;
    stream >> version;
    if (version == VERSION_1) {
        stream >> bitInfo.m_ranges;
        stream >> bitInfo.m_maxFrameWidth;
        stream >> bitInfo.m_metadata;
        stream >> bitInfo.m_rangeHighlights;
        return stream;
    }
    else {
        stream.setStatus(QDataStream::Status::ReadCorruptData);
        return stream;
    }
}
