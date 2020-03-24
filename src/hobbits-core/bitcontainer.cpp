#include "bitcontainer.h"
#include "pluginactionlineage.h"
#include "settingsmanager.h"
#include <QBuffer>
#include <QDebug>

BitContainer::BitContainer(QObject *parent) :
    QObject(parent),
    m_name("Some Bits"),
    m_id(QUuid::createUuid())
{
    setBitInfo(QSharedPointer<BitInfo>(new BitInfo()));
}

void BitContainer::setBits(QByteArray bytes, qint64 bitLen, QSharedPointer<BitInfo> bitInfo)
{
    QBuffer buffer(&bytes);
    buffer.open(QIODevice::ReadOnly);
    setBits(&buffer, bitLen, bitInfo);
}

void BitContainer::setBits(QIODevice *readableBytes, qint64 bitLen, QSharedPointer<BitInfo> bitInfo)
{
    setBits(QSharedPointer<BitArray>(new BitArray(readableBytes, bitLen)), bitInfo);
}

void BitContainer::setBits(QSharedPointer<const BitArray> bits, QSharedPointer<BitInfo> bitInfo)
{
    setBits(QSharedPointer<BitArray>(new BitArray(*bits.data())), bitInfo);
}

void BitContainer::setBits(QSharedPointer<BitArray> bits, QSharedPointer<BitInfo> bitInfo)
{
    m_bits = bits;

    if (bitInfo.isNull()) {
        // Initialize frames
        qint64 frameWidth = 256;
        QVector<Range> frames;
        for (qint64 i = 0; i < m_bits->sizeInBits(); i += frameWidth) {
            qint64 width = qMin(frameWidth - 1, m_bits->sizeInBits() - i - 1);
            frames.append(Range(i, i + width));
        }
        m_bitInfo->setFrames(frames);
    }
    else {
        m_bitInfo = bitInfo;
    }

    setBitInfo(m_bitInfo);
}

void BitContainer::setBitInfo(QSharedPointer<BitInfo> bitInfo)
{
    if (bitInfo.isNull()) {
        return;
    }

    m_bitInfo = QSharedPointer<BitInfo>(new BitInfo(*bitInfo.data()));
    m_bitInfo->setBits(m_bits);

    emit changed(this);
}

QSharedPointer<const BitArray> BitContainer::bits() const
{
    return m_bits;
}

QSharedPointer<const BitInfo> BitContainer::bitInfo() const
{
    return m_bitInfo;
}

QSharedPointer<BitInfo> BitContainer::bitInfo()
{
    return m_bitInfo;
}

QVector<Frame> BitContainer::frames() const
{
    if (m_bitInfo.isNull()) {
        return QVector<Frame>();
    }
    else {
        return m_bitInfo->frames();
    }
}

qint64 BitContainer::maxFrameWidth() const
{
    if (m_bitInfo.isNull()) {
        return 0;
    }
    else {
        return m_bitInfo->maxFrameWidth();
    }
}

void BitContainer::addHighlight(RangeHighlight highlight)
{
    m_bitInfo->addHighlight(highlight);
}

void BitContainer::addHighlights(QList<RangeHighlight> highlights)
{
    m_bitInfo->addHighlights(highlights);
}

void BitContainer::setMetadata(QString key, QVariant value)
{
    m_bitInfo->setMetadata(key, value);
}

void BitContainer::clearHighlightCategory(QString category)
{
    m_bitInfo->clearHighlightCategory(category);
}

QString BitContainer::name() const
{
    return m_name;
}

void BitContainer::setName(QString name)
{
    this->m_name = name;
    emit changed(this);
}

QSharedPointer<const PluginActionLineage> BitContainer::getActionLineage() const
{
    return m_actionLineage;
}

void BitContainer::setActionLineage(QSharedPointer<const PluginActionLineage> lineage)
{
    m_actionLineage = lineage;
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

    setBits(data, size);
    return true;
}

const QString VERSION_1 = "Really Good Hobbits Bit Container Serialization Format v1";
const QString VERSION_2 = "Oh So Good Hobbits Bit Container Serialization Format v2";
const QString VERSION_3 = "The Best Hobbits Bit Container Serialization Format v3";
QDataStream& operator<<(QDataStream &stream, const BitContainer &container)
{
    stream << VERSION_3;
    stream << *container.m_bitInfo.data();
    stream << container.bits()->getPreviewSize();
    stream << container.bits()->getPreviewBytes();

    return stream;
}

QDataStream& operator>>(QDataStream &stream, BitContainer &container)
{
    QString version;
    stream >> version;
    if (version == VERSION_3) {
        BitInfo bitInfo;
        stream >> bitInfo;
        int bitLength;
        QByteArray bytes;
        stream >> bitLength;
        stream >> bytes;
        container.setBits(bytes, bitLength, QSharedPointer<BitInfo>(new BitInfo(bitInfo)));
    }
    else if (version == VERSION_1 || version == VERSION_2) {
        QMap<QString, Range> highlights;
        QMap<QString, QString> metadata;
        stream >> highlights;
        stream >> metadata;
        int bitLength;
        QByteArray bytes;
        stream >> bitLength;
        stream >> bytes;
        container.setBits(bytes, bitLength);
    }
    else {
        stream.setStatus(QDataStream::Status::ReadCorruptData);
        return stream;
    }

    return stream;
}
