#include "bitcontainertreemodel.h"
#include "displayhelper.h"
#include <QPixmap>

BitContainerTreeModel::BitContainerTreeModel(QObject *parent) :
    QAbstractItemModel(parent)
{
    m_rootUuid = QUuid::createUuid();
    m_containerGroups.insert(m_rootUuid, {});
}

QVariant BitContainerTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }
    Q_ASSERT(index.model() == this);

    BitContainer *container = static_cast<BitContainer*>(index.internalPointer());
    if (role == Qt::DisplayRole) {
        qint64 size = container->size();
        float f_size;
        QString unit = "bits";
        if (size < 1000) {

            f_size = size;
            //QString unit = "bits";
        } else if (size >= 1000 && size < 8000) {

            unit = "bytes";
            f_size=size/8.0;
        } else if (size >= 8000 && size < 8000000) {

            unit = "kB";
            f_size=size/8000.0;
        } else if (size >= 8000000 && size < 8000000000) {

            unit = "MB";
            f_size=size/8000000.0;
        } else {

            unit = "GB";
            f_size=size/8000000000.0;
        }
        return QVariant(QString(container->name()+"\n%1 "+unit).arg(f_size));
    }
    else if (role == Qt::DecorationRole) {
        return QVariant(DisplayHelper::bitRasterThumbnail(container));
    }
    else if (role == Qt::UserRole) {
        return QVariant(container->id());
    }
    return QVariant();
}

Qt::ItemFlags BitContainerTreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return Qt::NoItemFlags;
    }

    return QAbstractItemModel::flags(index);
}

QVariant BitContainerTreeModel::headerData(
        int section,
        Qt::Orientation orientation,
        int role) const
{
    Q_UNUSED(section)
    Q_UNUSED(orientation)
    Q_UNUSED(role)
    return QVariant();
}

QModelIndex BitContainerTreeModel::index(
        int row,
        int column,
        const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent)) {
        return QModelIndex();
    }

    QUuid parentId = getIndexId(parent);

    auto group = m_containerGroups.value(parentId);
    if (group.size() > row) {
        auto container = group.at(row);
        return createIndex(row, column, container.data());
    }
    return QModelIndex();
}

QModelIndex BitContainerTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return QModelIndex();
    }

    BitContainer *childContainer = static_cast<BitContainer*>(index.internalPointer());

    return getContainerParentIndex(childContainer);
}

int BitContainerTreeModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() > 0) {
        return 0;
    }

    return m_containerGroups.value(getIndexId(parent)).size();
}

int BitContainerTreeModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 1;
}

void BitContainerTreeModel::updateAll()
{
    // TODO: this is supposed to do something?
}

QUuid BitContainerTreeModel::getIndexId(const QModelIndex &index) const
{
    auto container = getContainer(index);
    if (!container.isNull()) {
        return container->id();
    }
    return m_rootUuid;
}

QModelIndex BitContainerTreeModel::addContainer(QSharedPointer<BitContainer> bitContainer)
{
    QModelIndex idx = addContainerImpl(bitContainer);
    connect(bitContainer.data(), SIGNAL(changed()), this, SLOT(updateAll()));

    emit containerAdded(bitContainer);

    return idx;
}

void BitContainerTreeModel::removeContainer(const QModelIndex &index)
{
    BitContainer *container = static_cast<BitContainer*>(index.internalPointer());
    QModelIndex parentIndex = getContainerParentIndex(container);
    int row = getContainerRow(container);
    QList<QSharedPointer<BitContainer>> orphanedContainers;
    beginRemoveRows(parentIndex, row, row);
    // Get a copy of the container as a QSharedPointer so it doesn't get deleted when it is removed from the container
    QSharedPointer<BitContainer> containerPtr = m_containerMap.value(container->id());
    if (parentIndex.isValid()) {
        QUuid id = getContainer(parentIndex)->id();
        m_containerGroups[id].removeAt(row);
    }
    else {
        m_containerGroups[m_rootUuid].removeAt(row);
    }
    m_containerMap.remove(container->id());
    QList<QUuid> detach;
    for (QUuid childUuid : container->childUuids()) {
        detach.append(childUuid);
    }
    for (QUuid childUuid : detach) {
        if (m_containerMap.contains(childUuid)) {
            m_containerMap.value(childUuid)->detachParent(container->id());
            orphanedContainers.append(m_containerMap.value(childUuid));
            m_containerMap.remove(childUuid);
        }
    }
    detach.clear();
    m_containerGroups.remove(container->id());
    for (QUuid parentUuid : container->parentUuids()) {
        detach.append(parentUuid);
    }
    for (QUuid parentUuid : detach) {
        if (m_containerMap.contains(parentUuid)) {
            m_containerMap.value(parentUuid)->detachChild(container->id());
        }
    }
    endRemoveRows();

    for (auto orphan : orphanedContainers) {
        addContainerImpl(orphan);
    }
}


void BitContainerTreeModel::removeAllContainers()
{
    beginRemoveRows(QModelIndex(), 0, rowCount()-1);
    m_containerMap.clear();
    m_containerGroups.clear();
    m_containerGroups.insert(m_rootUuid, {});
    endRemoveRows();
}

QSharedPointer<BitContainer> BitContainerTreeModel::getContainer(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return QSharedPointer<BitContainer>();
    }

    BitContainer *indexContainer = static_cast<BitContainer*>(index.internalPointer());
    for (QSharedPointer<BitContainer> container : m_containerMap.values()) {
        if (container.data() == indexContainer) {
            return container;
        }
    }

    return QSharedPointer<BitContainer>();
}

QModelIndex BitContainerTreeModel::getContainerIndex(const QUuid &id) const
{
    if (!m_containerMap.contains(id)) {
        return QModelIndex();
    }
    auto container = m_containerMap.value(id);

    int row = getContainerRow(container.data());
    if (row >= 0) {
        return createIndex(row, 0, container.data());
    }
    return QModelIndex();
}

QSharedPointer<BitContainer> BitContainerTreeModel::getContainerById(QUuid id) const
{
    return m_containerMap.value(id);
}

QList<QSharedPointer<BitContainer>> BitContainerTreeModel::getContainers() const
{
    return m_containerMap.values();
}

int BitContainerTreeModel::getContainerRow(BitContainer *bitContainer) const
{
    QUuid parentId = m_rootUuid;
    if (!bitContainer->isRootContainer()) {
        parentId = bitContainer->parentUuids().at(0);
    }

    int row = 0;
    for (QSharedPointer<BitContainer> container : m_containerGroups.value(parentId)) {
        if (container.data() == bitContainer) {
            return row;
        }
        row++;
    }

    return -1;
}

QModelIndex BitContainerTreeModel::getContainerParentIndex(BitContainer *childContainer) const
{
    if (childContainer->isRootContainer()) {
        return QModelIndex();
    }

    return getContainerIndex(childContainer->parentUuids().at(0));
}

QModelIndex BitContainerTreeModel::addContainerImpl(QSharedPointer<BitContainer> bitContainer)
{
    m_containerMap.insert(bitContainer->id(), bitContainer);
    if (!m_containerGroups.contains(bitContainer->id())) {
        m_containerGroups.insert(bitContainer->id(), {});
    }

    if (bitContainer->isRootContainer()) {
        int row = rowCount(QModelIndex());
        beginInsertRows(QModelIndex(), row, row);
        m_containerGroups[m_rootUuid].append(bitContainer);
        endInsertRows();
        return this->index(row, 0, QModelIndex());
    }
    else {
        QUuid parentId = bitContainer->parentUuids().at(0);
        QModelIndex parentIndex = getContainerIndex(parentId);
        if (parentIndex.isValid()) {
            int row = rowCount(parentIndex);
            beginInsertRows(parentIndex, row, row);
            m_containerGroups[parentId].append(bitContainer);
            endInsertRows();
            return this->index(row, 0, parentIndex);
        }
    }

    return QModelIndex();
}
