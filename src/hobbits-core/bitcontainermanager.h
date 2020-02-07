#ifndef BITCONTAINERMANAGER_H
#define BITCONTAINERMANAGER_H

#include "bitcontainer.h"
#include "bitcontainertreemodel.h"
#include <QItemSelectionModel>
#include <QSharedPointer>

class BitContainerManager : public QObject
{
    Q_OBJECT

public:
    explicit BitContainerManager(QObject *parent = nullptr);

    QSharedPointer<BitContainer> getCurrentContainer();
    QSharedPointer<BitContainer> getContainerById(QUuid id);

    QSharedPointer<BitContainerTreeModel> getTreeModel();
    QSharedPointer<QItemSelectionModel> getCurrSelectionModel();

    void deleteCurrentContainer();

public slots:
signals:
    void currSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

private:
    QSharedPointer<BitContainerTreeModel> m_bitContainerTreeModel;
    QSharedPointer<QItemSelectionModel> m_currSelectionModel;
};

#endif // BITCONTAINERMANAGER_H
