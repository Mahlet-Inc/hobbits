#ifndef FREQUENCYPLOTWIDGET_H
#define FREQUENCYPLOTWIDGET_H

#include "displaybase.h"

class FrequencyPlotWidget : public DisplayBase
{
    Q_OBJECT

public:
    FrequencyPlotWidget(
            QSharedPointer<DisplayHandle> displayHandle,
            DisplayInterface *pluginRef,
            QWidget *parent = nullptr);

    void paintEvent(QPaintEvent*) override;
    void mouseMoveEvent(QMouseEvent *event) override;
};

#endif // FREQUENCYPLOTWIDGET_H