#ifndef BITCONTAINERMANAGERUI_H
#define BITCONTAINERMANAGERUI_H

#include "bitcontainertreemodel.h"
#include <QItemSelectionModel>
#include "bitcontainermanager.h"
#include "hobbits-widgets_global.h"

class HOBBITSWIDGETSSHARED_EXPORT BitContainerManagerUi : public BitContainerManager
{
    Q_OBJECT

public:
    explicit BitContainerManagerUi(QObject *parent = nullptr);

    QSharedPointer<BitContainerTreeModel> getTreeModel();
    QSharedPointer<QItemSelectionModel> getCurrSelectionModel();

    virtual QSharedPointer<BitContainer> currentContainer() override;
    virtual QSharedPointer<BitContainer> getContainerById(QUuid id) override;

    virtual bool addContainer(QSharedPointer<BitContainer> container) override;
    virtual bool selectContainer(QSharedPointer<BitContainer> container) override;

    virtual void deleteCurrentContainer() override;
    virtual void deleteAllContainers() override;

private Q_SLOTS:
    void manageSelectionChanged(const QItemSelection &selected, const QItemSelection &deselected);

private:
    QSharedPointer<BitContainerTreeModel> m_bitContainerTreeModel;
    QSharedPointer<QItemSelectionModel> m_currSelectionModel;
};

#endif // BITCONTAINERMANAGERUI_H
