#include "bitcontainer.h"
#include "pluginactionlineage.h"
#include "settingsmanager.h"
#include <QBuffer>

BitContainer::BitContainer() :
    QObject(),
    m_name("Some Bits"),
    m_nameWasSet(false),
    m_id(QUuid::createUuid())
{
    setActionLineage(PluginActionLineage::actionlessLineage());
}

QSharedPointer<BitContainer> BitContainer::create(QByteArray bytes, qint64 bitLen, QSharedPointer<const BitInfo> bitInfo)
{
    QBuffer buffer(&bytes);
    buffer.open(QIODevice::ReadOnly);
    return create(&buffer, bitLen, bitInfo);
}

QSharedPointer<BitContainer> BitContainer::create(QIODevice *readableBytes, qint64 bitLen, QSharedPointer<const BitInfo> bitInfo)
{
    return create(QSharedPointer<BitArray>(new BitArray(readableBytes, bitLen)), bitInfo);
}

QSharedPointer<BitContainer> BitContainer::create(QSharedPointer<const BitArray> bits, QSharedPointer<const BitInfo> bitInfo)
{
    return create(QSharedPointer<BitArray>(new BitArray(*bits.data())), bitInfo);
}

QSharedPointer<BitContainer> BitContainer::create(QSharedPointer<BitArray> bits, QSharedPointer<const BitInfo> bitInfo)
{
    auto container = QSharedPointer<BitContainer>(new BitContainer());
    container->m_bits = bits;
    container->setInfo(bitInfo);
    return container;
}

void BitContainer::setInfo(QSharedPointer<const BitInfo> bitInfo)
{
    m_mutex.lock();
    if (!m_info.isNull()) {
        disconnect(m_info.data(), nullptr, nullptr, nullptr);
    }
    m_info = BitInfo::create(m_bits->sizeInBits(), bitInfo);
    m_mutex.unlock();

    emit changed();

    connect(m_info.data(), &BitInfo::changed, [this]() {
        emit changed();
    });
}

QSharedPointer<const BitArray> BitContainer::bits() const
{
    return m_bits;
}

QSharedPointer<const BitInfo> BitContainer::info() const
{
    return m_info;
}

QSharedPointer<BitInfo> BitContainer::info()
{
    return m_info;
}

Frame BitContainer::frameAt(qint64 i) const
{
    return Frame(m_bits, m_info->frames()->at(i));
}

qint64 BitContainer::frameCount() const
{
    return m_info->frameCount();
}

qint64 BitContainer::maxFrameWidth() const
{
    return m_info->maxFrameWidth();
}

QString BitContainer::name() const
{
    return m_name;
}

void BitContainer::setName(QString name)
{
    m_mutex.lock();
    this->m_nameWasSet = true;
    this->m_name = name;
    m_mutex.unlock();
    emit changed();
}

bool BitContainer::nameWasSet() const
{
    return m_nameWasSet;
}

QSharedPointer<const PluginActionLineage> BitContainer::actionLineage() const
{
    return m_actionLineage;
}

QSharedPointer<PluginActionLineage> BitContainer::actionLineage()
{
    return m_actionLineage;
}

void BitContainer::setActionLineage(QSharedPointer<PluginActionLineage> lineage)
{
    QMutexLocker lock(&m_mutex);
    m_actionLineage = lineage;
}

bool BitContainer::isRootContainer() const
{
    return m_parents.isEmpty();
}

QList<QUuid> BitContainer::childUuids() const
{
    return m_children;
}

QList<QUuid> BitContainer::parentUuids() const
{
    return m_parents;
}

QUuid BitContainer::id() const
{
    return m_id;
}

void BitContainer::detachChild(QUuid childId)
{
    QMutexLocker lock(&m_mutex);
    m_children.removeAll(childId);
}

void BitContainer::detachParent(QUuid parentId)
{
    QMutexLocker lock(&m_mutex);
    m_parents.removeAll(parentId);
}

void BitContainer::addChild(QUuid childId)
{
    QMutexLocker lock(&m_mutex);
    m_children.append(childId);
}

void BitContainer::addParent(QUuid parentId)
{
    QMutexLocker lock(&m_mutex);
    m_parents.append(parentId);
}
