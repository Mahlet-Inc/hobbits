#ifndef PEAKSELECTOR_H
#define PEAKSELECTOR_H

#include <QScrollBar>
#include <QSlider>
#include <QWidget>

class PeakSelector : public QWidget
{
    Q_OBJECT

public:
    explicit PeakSelector(QWidget *parent = nullptr);

    QScrollBar* getHScroll();
    QSlider* getZoomSlider();

protected:
    void paintEvent(QPaintEvent *event) override;

    void mouseMoveEvent(QMouseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

public slots:
    void setData(QVector<QPointF> data);
    QPointF getPeak();

    void adjustScroll();

signals:
    void peakSelected(QPointF);

private:
    QVector<QPointF> m_data;
    double m_dataYMax;
    double m_dataYMin;
    QScrollBar *m_hScroll;
    QSlider *m_zoomSlider;

    int m_startDrag;
    int m_endDrag;

    bool m_disabled;
    bool m_hasNan;
};

#endif // PEAKSELECTOR_H
