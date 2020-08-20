#include "bitcontainerlistmodel.h"
#include "displayhelper.h"
#include <QPixmap>

BitContainerListModel::BitContainerListModel(QObject *parent) :
    QAbstractListModel(parent)
{

}

QVariant BitContainerListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()) {
        return QVariant();
    }
    Q_ASSERT(index.model() == this);

    if (role == Qt::DisplayRole) {
        return QVariant(m_bitContainers.at(index.row())->name());
    }
    else if (role == Qt::DecorationRole) {
        return QVariant(DisplayHelper::bitRasterThumbnail(m_bitContainers.at(index.row())));
    }
    return QVariant();
}

int BitContainerListModel::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()) {
        return 0;
    }
    return m_bitContainers.size();
}

QVariant BitContainerListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    Q_UNUSED(section)
    Q_UNUSED(orientation)
    Q_UNUSED(role)
    return QVariant();
}

QModelIndex BitContainerListModel::addContainer(QSharedPointer<BitContainer> bitContainer)
{
    beginInsertRows(QModelIndex(), rowCount(), rowCount());
    m_bitContainers.append(bitContainer);
    endInsertRows();
    connect(bitContainer.data(), SIGNAL(changed()), this, SLOT(updateAll()));

    return this->index(m_bitContainers.size() - 1);
}

QSharedPointer<BitContainer> BitContainerListModel::getContainer(const QModelIndex &index) const
{
    if (!index.isValid()) {
        return QSharedPointer<BitContainer>();
    }
    return m_bitContainers.at(index.row());
}

void BitContainerListModel::updateAll()
{
    QModelIndex top = this->index(0);
    QModelIndex bottom = this->index(m_bitContainers.size() - 1);
    emit dataChanged(top, bottom, {Qt::DisplayRole, Qt::DecorationRole});
}

void BitContainerListModel::removeContainer(int row)
{
    beginRemoveRows(QModelIndex(), row, row);
    m_bitContainers.removeAt(row);
    endRemoveRows();
}

QList<QSharedPointer<BitContainer>> BitContainerListModel::getContainers() const
{
    return m_bitContainers;
}
