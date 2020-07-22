#ifndef BITCONTAINERMANAGER_H
#define BITCONTAINERMANAGER_H

#include "bitcontainer.h"
#include "bitcontainertreemodel.h"
#include <QItemSelectionModel>
#include <QSharedPointer>

#include "hobbits-core_global.h"

class HOBBITSCORESHARED_EXPORT BitContainerManager : public QObject
{
    Q_OBJECT

public:
    explicit BitContainerManager(QObject *parent = nullptr);

    QSharedPointer<BitContainer> getCurrentContainer();
    QSharedPointer<BitContainer> getContainerById(QUuid id);

    QSharedPointer<BitContainerTreeModel> getTreeModel();
    QSharedPointer<QItemSelectionModel> getCurrSelectionModel();

    void deleteCurrentContainer();
    void deleteAllContainers();

private Q_SLOTS:
    void manageSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

Q_SIGNALS:
    void currSelectionChanged(QSharedPointer<BitContainer> selected, QSharedPointer<BitContainer> deselected);
    void containerAdded(QSharedPointer<BitContainer>);

private:
    QSharedPointer<BitContainerTreeModel> m_bitContainerTreeModel;
    QSharedPointer<QItemSelectionModel> m_currSelectionModel;
};

#endif // BITCONTAINERMANAGER_H
