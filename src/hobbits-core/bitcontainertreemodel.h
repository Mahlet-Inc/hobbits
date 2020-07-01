#ifndef BITCONTAINERTREEMODEL_H
#define BITCONTAINERTREEMODEL_H

#include "bitcontainer.h"
#include <QAbstractItemModel>

#include "hobbits-core_global.h"

class HOBBITSCORESHARED_EXPORT BitContainerTreeModel : public QAbstractItemModel
{
    Q_OBJECT

public:
    explicit BitContainerTreeModel(QObject *parent = nullptr);

    QVariant data(const QModelIndex &index, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
    QVariant headerData(
            int section,
            Qt::Orientation orientation,
            int role = Qt::DisplayRole) const override;
    QModelIndex index(
            int row,
            int column,
            const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;


    QModelIndex addContainer(QSharedPointer<BitContainer> bitContainer);
    void removeContainer(const QModelIndex &index);
    void removeAllContainers();
    QSharedPointer<BitContainer> getContainer(const QModelIndex &index) const;

    QList<QSharedPointer<BitContainer>> getContainers() const;
    QSharedPointer<BitContainer> getContainerById(QUuid id) const;

signals:
public slots:
    void updateAll();

private:
    int getContainerRow(BitContainer *bitContainer) const;
    QModelIndex getContainerParentIndex(BitContainer *bitContainer) const;

    QMap<QUuid, QSharedPointer<BitContainer>> m_containerMap;
};

#endif // BITCONTAINERTREEMODEL_H
