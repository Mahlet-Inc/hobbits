#ifndef BATCHEDITLINK_H
#define BATCHEDITLINK_H

#include "batchedititem.h"

class BatchEditLink: public QGraphicsObject
{
    Q_OBJECT
public:
    explicit BatchEditLink(BatchEditItem* sender, BatchEditItem* receiver, int outputNumber);
    ~BatchEditLink() override;

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;
    QRectF boundingRect() const override;
    QPainterPath shape() const override;

public slots:
    void deactivate(QUuid deleted);
    void checkRemoval(QUuid deleted, int outputNumber);

private:
    BatchEditItem* m_sender;
    BatchEditItem* m_receiver;
    int m_outputNumber;
    bool m_active;
};

#endif // BATCHEDITLINK_H
