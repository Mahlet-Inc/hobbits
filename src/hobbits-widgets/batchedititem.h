#ifndef BATCHEDITITEM_H
#define BATCHEDITITEM_H

#include "pluginactionbatch.h"
#include "batchedititemwidget.h"
#include <QGraphicsObject>

class BatchEditItem : public QGraphicsObject
{
    Q_OBJECT
public:
    BatchEditItem(QSharedPointer<HobbitsPluginManager> pluginManager, QSharedPointer<const PluginActionBatch::ActionStep> step);
    ~BatchEditItem() override;
    static BatchEditItem* fromMimeData(QSharedPointer<HobbitsPluginManager> pluginManager, const QMimeData *data);

    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

    QSharedPointer<PluginActionBatch::ActionStep> toStep() const;
    QList<QPair<QUuid, int> > inputs() const;
    QString displayString() const;

    QUuid id() const;

public slots:
    void removeAllInputs();
    void removeInput(QUuid id, int outputNumber);
    void addInput(QUuid id, int outputNumber);

signals:
    void deleted(QUuid);
    void inputPressed(QUuid);
    void outputPressed(QUuid);
    void inputRemoved(QUuid, int);

private:
    QUuid m_id;
    QString m_pluginName;
    PluginAction::PluginType m_pluginType;
    QList<QPair<QUuid, int>> m_inputs;

    BatchEditItemWidget *m_editor;
    QGraphicsProxyWidget *m_proxyWidget;
};

#endif // BATCHEDITITEM_H
