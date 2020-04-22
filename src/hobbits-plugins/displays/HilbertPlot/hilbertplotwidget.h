#ifndef HILBERTPLOTWIDGET_H
#define HILBERTPLOTWIDGET_H

#include "displaybase.h"

class HilbertPlotWidget : public DisplayBase
{
    Q_OBJECT

public:
    HilbertPlotWidget(
            QSharedPointer<DisplayHandle> displayHandle,
            DisplayInterface *pluginRef,
            QWidget *parent = nullptr);

    void paintEvent(QPaintEvent*) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    void rotate(QPoint &p, int n, bool rx, bool ry);
    QPoint toHilbertCoordinate(int n, int idx);
    QVector<QPoint> getPointsForOrder(int order);

    QMap<int, QVector<QPoint>> m_hilbertPoints;
    QList<int> m_hilbertRecursionOrders;
};

#endif // HILBERTPLOTWIDGET_H
