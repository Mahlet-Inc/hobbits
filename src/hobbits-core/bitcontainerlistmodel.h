#ifndef BITCONTAINERLISTMODEL_H
#define BITCONTAINERLISTMODEL_H

#include "bitcontainer.h"
#include <QAbstractListModel>

#include "hobbits-core_global.h"

class HOBBITSCORESHARED_EXPORT BitContainerListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit BitContainerListModel(QObject *parent = nullptr);

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    QModelIndex addContainer(QSharedPointer<BitContainer> bitContainer);
    void removeContainer(int row);
    QSharedPointer<BitContainer> getContainer(const QModelIndex &index) const;

    QList<QSharedPointer<BitContainer>> getContainers() const;

private:
    QList<QSharedPointer<BitContainer>> m_bitContainers;

public slots:
    void updateAll();

};

#endif // BITCONTAINERLISTMODEL_H
