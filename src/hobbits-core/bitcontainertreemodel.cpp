#include "bitcontainertreemodel.h"
#include "displayhelper.h"

BitContainerTreeModel::BitContainerTreeModel(QObject *parent) :
    QAbstractItemModel(parent)
{

}

QVariant BitContainerTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }
    Q_ASSERT(index.model() == this);

    BitContainer *container = static_cast<BitContainer*>(index.internalPointer());
    if (role == Qt::DisplayRole) {
        return QVariant(container->name());
    }
    else if (role == Qt::DecorationRole) {
        return QVariant(DisplayHelper::bitRasterThumbnail(container));
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

    if (!parent.isValid()) {
        int rootContainers = 0;
        for (QSharedPointer<BitContainer> container : m_containerMap.values()) {
            if (container->isRootContainer()) {
                if (rootContainers == row) {
                    return createIndex(row, column, container.data());
                }
                else {
                    rootContainers++;
                }
            }
        }
    }
    else {
        BitContainer *parentContainer = static_cast<BitContainer*>(parent.internalPointer());
        int currentChild = 0;
        for (QUuid childUuid : parentContainer->getChildUuids()) {
            if (m_containerMap.contains(childUuid)) {
                if (currentChild == row) {
                    return createIndex(row, column, m_containerMap.value(childUuid).data());
                }
                else {
                    currentChild++;
                }
            }
        }
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

    if (!parent.isValid()) {
        int rootContainers = 0;
        for (QSharedPointer<BitContainer> container : m_containerMap.values()) {
            if (container->isRootContainer()) {
                rootContainers++;
            }
        }
        return rootContainers;
    }
    else {
        BitContainer *parentContainer = static_cast<BitContainer*>(parent.internalPointer());
        // maybe needs to make sure they are there?
        return parentContainer->getChildUuids().length();
    }
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

QModelIndex BitContainerTreeModel::addContainer(QSharedPointer<BitContainer> bitContainer)
{
    m_containerMap.insert(bitContainer->getId(), bitContainer);
    QModelIndex parentIndex = getContainerParentIndex(bitContainer.data());
    int row = getContainerRow(bitContainer.data());
    beginInsertRows(parentIndex, row, row);
    endInsertRows();
    connect(bitContainer.data(), SIGNAL(changed()), this, SLOT(updateAll()));

    emit containerAdded(bitContainer);

    return this->index(row, 0, parentIndex);
}

void BitContainerTreeModel::removeContainer(const QModelIndex &index)
{
    BitContainer *container = static_cast<BitContainer*>(index.internalPointer());
    QModelIndex parentIndex = getContainerParentIndex(container);
    int row = getContainerRow(container);
    QList<QSharedPointer<BitContainer>> orphanedContainers;
    beginRemoveRows(parentIndex, row, row);
    // Get a copy of the container as a QSharedPointer so it doesn't get deleted when it is removed from the container
    QSharedPointer<BitContainer> containerPtr = m_containerMap.value(container->getId());
    m_containerMap.remove(container->getId());
    QList<QUuid> detach;
    for (QUuid childUuid : container->getChildUuids()) {
        detach.append(childUuid);
    }
    for (QUuid childUuid : detach) {
        if (m_containerMap.contains(childUuid)) {
            m_containerMap.value(childUuid)->detachParent(container->getId());
            orphanedContainers.append(m_containerMap.value(childUuid));
        }
    }
    detach.clear();
    for (QUuid parentUuid : container->getParentUuids()) {
        detach.append(parentUuid);
    }
    for (QUuid parentUuid : detach) {
        if (m_containerMap.contains(parentUuid)) {
            m_containerMap.value(parentUuid)->detachChild(container->getId());
        }
    }
    endRemoveRows();
    if (!orphanedContainers.isEmpty()) {
        beginInsertRows(QModelIndex(), 0, rowCount() - 1);
        endInsertRows();
    }
}


void BitContainerTreeModel::removeAllContainers()
{
    beginRemoveRows(QModelIndex(), 0, rowCount()-1);
    m_containerMap.clear();
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
    if (bitContainer->isRootContainer()) {
        int row = 0;
        for (QSharedPointer<BitContainer> container : m_containerMap.values()) {
            if (container->isRootContainer()) {
                if (container.data() == bitContainer) {
                    return row;
                }
                row++;
            }
        }
    }
    else {
        for (QUuid parentUuid : bitContainer->getParentUuids()) {
            if (m_containerMap.contains(parentUuid)) {
                return m_containerMap.value(parentUuid)->getChildUuids().indexOf(bitContainer->getId());
            }
        }
    }
    return -1;
}

QModelIndex BitContainerTreeModel::getContainerParentIndex(BitContainer *childContainer) const
{
    if (childContainer->isRootContainer()) {
        return QModelIndex();
    }

    for (QUuid parentUuid : childContainer->getParentUuids()) {
        if (m_containerMap.contains(parentUuid)) {
            BitContainer *parentContainer = m_containerMap.value(parentUuid).data();
            int row = getContainerRow(parentContainer);
            if (row >= 0) {
                return createIndex(getContainerRow(parentContainer), 0, parentContainer);
            }
        }
    }

    return QModelIndex();
}
