#include "batcheditlink.h"
#include <QPen>
#include <QPainter>
#include <QPainterPath>

BatchEditLink::BatchEditLink(BatchEditItem *sender, BatchEditItem *receiver, int outputNumber) :
    m_sender(sender),
    m_receiver(receiver),
    m_outputNumber(outputNumber),
    m_active(true)
{

    setFlags(QGraphicsItem::ItemIsSelectable
             | QGraphicsItem::ItemIsFocusable);

    setAcceptHoverEvents(true);


    setPos(0,0);

    connect(sender, &BatchEditItem::deleted, this, &BatchEditLink::deactivate);
    connect(receiver, &BatchEditItem::inputRemoved, this, &BatchEditLink::checkRemoval);
    connect(receiver, &BatchEditItem::deleted, this, &BatchEditLink::deactivate);

    receiver->addInput(sender->id(), outputNumber);
}

BatchEditLink::~BatchEditLink()
{
    deactivate(QUuid());
}

void BatchEditLink::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if (!m_active) {
        return;
    }

    QPen normal(Qt::black, 2);
    QPen highlighted(QColor(40, 130, 230), 3);

    painter->setPen(this->hasFocus() ? highlighted : normal);
    painter->drawLine(mapFromScene(
                          m_sender->mapToScene(
                              QPointF(m_sender->boundingRect().right(),
                                      m_sender->boundingRect().center().y()))),
                      mapFromScene(
                          m_receiver->mapToScene(
                              QPointF(m_receiver->boundingRect().left(),
                                      m_receiver->boundingRect().center().y()))));
}

QRectF BatchEditLink::boundingRect() const
{
    return shape().boundingRect();
}

QPainterPath BatchEditLink::shape() const
{
    if (!m_active) {
        return QPainterPath();
    }

    QRectF sender = mapFromScene(m_sender->mapToScene(m_sender->boundingRect()).boundingRect()).boundingRect();
    QRectF receiver = mapFromScene(m_receiver->mapToScene(m_receiver->boundingRect()).boundingRect()).boundingRect();

    QPainterPath path(QPointF(sender.center().y() - 10, sender.right()));
    path.lineTo(QPointF(sender.center().y() + 10, sender.right()));
    path.lineTo(QPointF(receiver.center().y() + 10, receiver.left()));
    path.lineTo(QPointF(receiver.center().y() - 10, receiver.left()));
    path.closeSubpath();

    return path;
}

void BatchEditLink::deactivate(QUuid deleted)
{
    Q_UNUSED(deleted)
    if (m_active) {
        m_active = false;
        m_receiver->removeInput(m_sender->id(), m_outputNumber);
        deleteLater();
    }
}

void BatchEditLink::checkRemoval(QUuid deleted, int outputNumber)
{
    if (m_active && m_sender->id() == deleted && outputNumber == m_outputNumber) {
        deactivate(QUuid());
    }
}
