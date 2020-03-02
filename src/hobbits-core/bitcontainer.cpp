#include "bitcontainer.h"
#include "pluginactionlineage.h"
#include "settingsmanager.h"
#include <QDebug>
#include <QBuffer>

BitContainer::BitContainer(QObject *parent) :
    QObject(parent),
    m_name("Some Bits"),
    m_id(QUuid::createUuid()),
    m_lastFrameOffsetFocus(0),
    m_lastBitOffsetFocus(0)
{

}

QList<Frame> BitContainer::getFrames() const
{
    return m_frames;
}

qint64 BitContainer::getMaxFrameWidth() const
{
    return m_maxFrameWidth;
}


void BitContainer::setBytes(QByteArray bytes, qint64 bitLen)
{
    QBuffer buffer(&bytes);
    buffer.open(QIODevice::ReadOnly);
    setBytes(&buffer, bitLen);
}

void BitContainer::setBytes(QIODevice* readableBytes, qint64 bitLen)
{
    setBytes(QSharedPointer<BitArray>(new BitArray(readableBytes, bitLen)));
}

void BitContainer::setBytes(QSharedPointer<BitArray> bits)
{
    m_bits = bits;

    // Initialize frames
    qint64 frameWidth = 256;
    QList<Frame> frames;

    for (qint64 i = 0; i < m_bits->sizeInBits(); i += frameWidth) {
        qint64 width = qMin(frameWidth - 1, m_bits->sizeInBits() - i - 1);
        frames.append(Frame(m_bits, i, i + width));
    }

    this->setFrames(frames, frameWidth);
}

void BitContainer::setFrames(QList<Frame> frames, qint64 maxFrameWidth)
{
    m_frames = frames;

    if (maxFrameWidth < 1) {
        maxFrameWidth = 0;
        for (Frame frame : m_frames) {
            maxFrameWidth = qMax(maxFrameWidth, frame.size());
        }
    }
    m_maxFrameWidth = maxFrameWidth;

    emit framesChanged(this);
    emit changed(this);
}

QImage BitContainer::getRasterImage(qint64 x, qint64 y, int w, int h) const
{
    QColor trueColor = SettingsManager::getInstance().getUiSetting(SettingsData::ONE_COLOR_KEY).value<QColor>();
    QColor falseColor = SettingsManager::getInstance().getUiSetting(SettingsData::ZERO_COLOR_KEY).value<QColor>();
    QImage raster(w, h, QImage::Format_ARGB32);
    raster.fill(qRgba(0x66, 0x66, 0x66, 0x66));

    qint64 frameOffset = y;
    qint64 bitOffset = x;

    if (frameOffset < 0) {
        return raster;
    }

    for (int i = 0; i < h; i++) {
        if (i + frameOffset >= getFrames().size()) {
            break;
        }
        Frame frame = getFrames().at(int(i + frameOffset));

        for (int ii = 0; ii < w; ii++) {
            if (ii + bitOffset >= frame.size()) {
                break;
            }

            if (frame.at(ii + bitOffset)) {
                raster.setPixel(ii, i, trueColor.rgba());
            }
            else {
                raster.setPixel(ii, i, falseColor.rgba());
            }
        }
    }
    return raster;
}

QImage BitContainer::getByteRasterImage(qint64 x, qint64 y, int w, int h) const
{
    QImage raster(w, h, QImage::Format_ARGB32);
    raster.fill(qRgba(0x66, 0x66, 0x66, 0x66));

    qint64 frameOffset = y;
    qint64 byteOffset = x / 8;
    qint64 bitOffset = byteOffset * 8;

    if (frameOffset < 0) {
        return raster;
    }

    for (int i = 0; i < h; i++) {
        if (i + frameOffset >= getFrames().size()) {
            break;
        }
        Frame frame = getFrames().at(int(i + frameOffset));

        for (int ii = 0; ii < w * 8; ii += 8) {
            if (ii + bitOffset + 8 >= frame.size()) {
                break;
            }

            quint8 byteVal = 0;
            for (int bit = 0; bit < 8; bit++) {
                if (frame.at(ii + bit + bitOffset)) {
                    byteVal |= 0x80 >> bit;
                }
            }

            QColor c = SettingsManager::getInstance().getUiSetting(SettingsData::BYTE_HUE_SAT_KEY).value<QColor>();
            c.setHsl(c.hue(), c.saturation(), byteVal);
            raster.setPixel(ii / 8, i, c.rgba());
        }
    }
    return raster;
}

QString BitContainer::getName() const
{
    return m_name;
}

QPixmap BitContainer::getThumbnail()
{
    return QPixmap::fromImage(this->getRasterImage(0, 0, 64, 64));
}

void BitContainer::setName(QString name)
{
    this->m_name = name;
    emit changed(this);
}

QSharedPointer<const BitArray> BitContainer::getBaseBits() const
{
    return m_bits;
}

QList<Range> BitContainer::getHighlights(QString type) const
{
    return m_highlightMap.value(type);
}

void BitContainer::setHighlights(QString type, QList<Range> highlights)
{
    m_highlightMap.insert(type, highlights);
    emit highlightsChanged(this);
    emit changed(this);
}

int BitContainer::getFrameOffsetContaining(Range target) const
{
    if (m_frames.isEmpty()) {
        return -1;
    }
    int offset = m_frames.size() / 2;
    int index = offset;
    int countdown = 7;
    while (true) {
        unsigned int compare = target.compare(m_frames.at(index));

        if (compare & Frame::Overlapping) {
            return index;
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

        if (index < 0 || index >= m_frames.size()) {
            return -1;
        }
    }
}

QSharedPointer<const PluginActionLineage> BitContainer::getActionLineage() const
{
    return m_actionLineage;
}

void BitContainer::setActionLineage(QSharedPointer<const PluginActionLineage> lineage)
{
    m_actionLineage = lineage;
}

QStringList BitContainer::getMetadata(QString key) const
{
    return m_metadata.value(key);
}

void BitContainer::setMetadata(QString key, QStringList value)
{
    m_metadata.insert(key, value);
}

int BitContainer::getLastBitOffsetFocus() const
{
    return m_lastBitOffsetFocus;
}

int BitContainer::getLastFrameOffsetFocus() const
{
    return m_lastFrameOffsetFocus;
}

void BitContainer::requestFocus(int bitOffset, int frameOffset)
{
    emit focusRequested(bitOffset, frameOffset);
}

void BitContainer::recordFocus(int bitOffset, int frameOffset)
{
    m_lastFrameOffsetFocus = frameOffset;
    m_lastBitOffsetFocus = bitOffset;
}

void BitContainer::frameViaHighlights()
{
    QList<Frame> frames;
    for (auto key : m_highlightMap.keys()) {
        if (key == "frames") {
            for (Range range : m_highlightMap.value(key)) {
                frames.append(Frame(getBaseBits(), range));
            }
        }
    }

    int maxFrameWidth = -1;
    for (auto key : m_metadata.keys()) {
        if (key == "max_frame_width") {
            maxFrameWidth = m_metadata.value(key).at(0).toInt();
        }
    }

    if (!frames.isEmpty()) {
        setFrames(frames, maxFrameWidth);
    }
}

bool BitContainer::isRootContainer() const
{
    return m_parents.isEmpty();
}

QList<QUuid> BitContainer::getChildUuids() const
{
    return m_children;
}

QList<QUuid> BitContainer::getParentUuids() const
{
    return m_parents;
}

QUuid BitContainer::getId() const
{
    return m_id;
}

void BitContainer::detachChild(QUuid childId)
{
    m_children.removeAll(childId);
}

void BitContainer::detachParent(QUuid parentId)
{
    m_parents.removeAll(parentId);
}

void BitContainer::addChild(QUuid childId)
{
    m_children.append(childId);
}

void BitContainer::addParent(QUuid parentId)
{
    m_parents.append(parentId);
}

QJsonDocument BitContainer::serializeJson() const
{
    QJsonObject root;
    root.insert("size", m_bits->getPreviewSize());
    root.insert("data", QString(m_bits->getPreviewBytes().toBase64()));
    return QJsonDocument(root);
}

bool BitContainer::deserializeJson(QJsonDocument json)
{
    QJsonObject root = json.object();
    if (!(root.contains("size")
          && root.value("size").isDouble()
          && root.contains("data")
          && root.value("data").isString())) {
        return false;
    }

    QByteArray data = QByteArray::fromBase64(root.value("data").toString().toLocal8Bit());
    qint64 size = root.value("size").toInt();
    if (size > data.length() * 8) {
        return false;
    }

    setBytes(data, size);
    return true;
}

const QString VERSION_1 = "Really Good Hobbits Bit Container Serialization Format v1";
const QString VERSION_2 = "Oh So Good Hobbits Bit Container Serialization Format v2";
QDataStream& operator<<(QDataStream &stream, const BitContainer &container)
{
    stream << VERSION_2;
    stream << container.m_highlightMap;
    stream << container.m_metadata;
    stream << container.getBaseBits()->getPreviewSize();
    stream << container.getBaseBits()->getPreviewBytes();

    return stream;
}

QDataStream& operator>>(QDataStream &stream, BitContainer &container)
{
    QString version;
    stream >> version;
    if (version == VERSION_1 || version == VERSION_2) {
        stream >> container.m_highlightMap;
        stream >> container.m_metadata;
        int bitLength;
        QByteArray bytes;
        stream >> bitLength;
        stream >> bytes;
        container.setBytes(bytes, bitLength);
        container.frameViaHighlights();
    }
    else {
        stream.setStatus(QDataStream::Status::ReadCorruptData);
        return stream;
    }

    return stream;
}
