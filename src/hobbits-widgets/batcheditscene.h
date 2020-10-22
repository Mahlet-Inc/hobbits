#ifndef BATCHEDITSCENE_H
#define BATCHEDITSCENE_H

#include <QGraphicsScene>
#include <QMimeData>
#include <QKeyEvent>
#include "hobbitspluginmanager.h"
#include "batchedititem.h"
#include "batcheditlink.h"
#include "hobbits-widgets_global.h"

class HOBBITSWIDGETSSHARED_EXPORT BatchEditScene : public QGraphicsScene
{
public:
    BatchEditScene(QSharedPointer<HobbitsPluginManager> pluginManager);

    void dragEnterEvent(QGraphicsSceneDragDropEvent *event) override;
    void dragLeaveEvent(QGraphicsSceneDragDropEvent *event) override;
    void dragMoveEvent(QGraphicsSceneDragDropEvent *event) override;
    void dropEvent(QGraphicsSceneDragDropEvent *event) override;

    void mouseMoveEvent(QGraphicsSceneMouseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

    QSharedPointer<PluginActionBatch> getBatch();
    void setBatch(QSharedPointer<PluginActionBatch> batch);
    void resetBatch();

private slots:
    void itemDeleted(QUuid id);
    void itemInputSelected(QUuid id);
    void itemOutputSelected(QUuid id);

    void createLink(QUuid output, QUuid input, int outputNumber = 0);

private:
    bool isPluginData(const QMimeData* data);
    void addBatchEditItem(BatchEditItem* item);
    QMap<QUuid, QString> namesForIds();

    QSharedPointer<HobbitsPluginManager> m_pluginManager;
    QMap<QUuid, BatchEditItem*> m_itemMap;
    QMap<QUuid, QPair<QUuid, BatchEditLink*>> m_linkMap;

    QPair<QUuid, int> m_pendingConnection;
};

#endif // BATCHEDITSCENE_H
