#include "batcheditscene.h"
#include <QGraphicsSceneDragDropEvent>
#include "batchedititem.h"
#include "batchinputdialog.h"

BatchEditScene::BatchEditScene(QSharedPointer<HobbitsPluginManager> pluginManager) :
    m_pluginManager(pluginManager)
{
    m_pendingConnection = {QUuid(), -1};
}

void BatchEditScene::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
    if (isPluginData(event->mimeData())) {
        event->accept();
    }
    else {
        event->ignore();
    }
}

void BatchEditScene::dragLeaveEvent(QGraphicsSceneDragDropEvent *event)
{
    event->accept();
}

void BatchEditScene::dragMoveEvent(QGraphicsSceneDragDropEvent *event)
{
    if (isPluginData(event->mimeData())) {
        event->accept();
    }
    else {
        event->ignore();
    }
}

void BatchEditScene::dropEvent(QGraphicsSceneDragDropEvent *event)
{
    if (isPluginData(event->mimeData())) {
        event->accept();

        auto item = BatchEditItem::fromMimeData(m_pluginManager, event->mimeData());
        addBatchEditItem(item);
        item->setPos(event->scenePos());
        update();
    }
    else {
        event->ignore();
    }
}

void BatchEditScene::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
    QGraphicsScene::mouseMoveEvent(event);
}

void BatchEditScene::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Delete)
    {
        QGraphicsItem *item = focusItem();

        if (item)
        {
            BatchEditItem* editItem = dynamic_cast<BatchEditItem*>(item);
            if (editItem)
            {
                setFocusItem(nullptr);
                this->removeItem(editItem);
                m_itemMap.remove(editItem->id());
                editItem->deleteLater();
                update();
                return;
            }


            BatchEditLink* linkItem = dynamic_cast<BatchEditLink*>(item);
            if (linkItem)
            {
                setFocusItem(nullptr);
                this->removeItem(linkItem);
                linkItem->deleteLater();
                update();
                return;
            }
        }
    }

    QGraphicsScene::keyPressEvent(event);
}

QSharedPointer<PluginActionBatch> BatchEditScene::getBatch()
{
    QList<QSharedPointer<const PluginActionBatch::ActionStep>> steps;
    for (auto item: m_itemMap.values()) {
        steps.append(item->toStep());
    }
    QSharedPointer<PluginActionBatch> batch(new PluginActionBatch(steps));
    return batch;
}

void BatchEditScene::setBatch(QSharedPointer<PluginActionBatch> batch)
{
    resetBatch();

    for (auto step: batch->actionSteps()) {
        auto item = new BatchEditItem(m_pluginManager, step);
        addBatchEditItem(item);
        item->setPos(step->editorPosition);
    }

    QList<QPair<QUuid, int>> removals;
    for (auto item: m_itemMap) {
        removals.clear();
        auto inputs = item->inputs();
        item->removeAllInputs(); // reset inputs, otherwise they get duped by createLink
        for (auto input: inputs) {
            if (!m_itemMap.contains(input.first)) {
                // TODO: warn of ignored inputs?
                removals.append(input);
                continue;
            }
            createLink(input.first, item->id());
        }
        for (auto removal: removals) {
            item->removeInput(removal.first, removal.second);
        }
    }
}

void BatchEditScene::resetBatch()
{
    for (auto item: m_itemMap.values()) {
        delete item;
    }
    m_itemMap.clear();
}

void BatchEditScene::itemDeleted(QUuid id)
{
    Q_UNUSED(id)
}

void BatchEditScene::itemInputSelected(QUuid id)
{
    if (m_pendingConnection.first == id) {
        m_pendingConnection = {QUuid(), -1};
        update();
    }
    else if (m_pendingConnection.second == -1) {
        auto inputItem = m_itemMap.value(id);
        QScopedPointer<BatchInputDialog> dialog(new BatchInputDialog(inputItem->inputs(), namesForIds()));
        if (dialog->exec()) {
            inputItem->removeAllInputs();
            for (auto input : dialog->inputs()) {
                createLink(input.first, id, input.second);
            }
            update();
        }
    }
    else {
        createLink(m_pendingConnection.first, id);
        m_pendingConnection = {QUuid(), -1};
        update();
    }
}

void BatchEditScene::itemOutputSelected(QUuid id)
{
    if (m_pendingConnection.second == -1) {
        m_pendingConnection = {id, 1};
        update();
    }
    else {
        m_pendingConnection = {QUuid(), -1};
        update();
    }
}

void BatchEditScene::createLink(QUuid outId, QUuid inId, int outputNumber)
{
    auto output = m_itemMap.value(outId);
    auto input = m_itemMap.value(inId);
    auto link = new BatchEditLink(output, input, outputNumber);
    link->setZValue(-1);
    addItem(link);
}

bool BatchEditScene::isPluginData(const QMimeData *data)
{
    return data->hasFormat("text/hobbits/plugin/operator")
            || data->hasFormat("text/hobbits/plugin/analyzer")
            || data->hasFormat("text/hobbits/plugin/importer")
            || data->hasFormat("text/hobbits/plugin/exporter")
            || data->hasFormat("text/hobbits/batchinput");
}

void BatchEditScene::addBatchEditItem(BatchEditItem *item)
{
    connect(item, &BatchEditItem::deleted, this, &BatchEditScene::itemDeleted);
    connect(item, &BatchEditItem::inputPressed, this, &BatchEditScene::itemInputSelected);
    connect(item, &BatchEditItem::outputPressed, this, &BatchEditScene::itemOutputSelected);
    addItem(item);
    m_itemMap.insert(item->id(), item);
}

QMap<QUuid, QString> BatchEditScene::namesForIds()
{
    QMap<QUuid, QString> nameMap;
    for (auto id: m_itemMap.keys()) {
        nameMap.insert(id, m_itemMap.value(id)->displayString());
    }
    return nameMap;
}
