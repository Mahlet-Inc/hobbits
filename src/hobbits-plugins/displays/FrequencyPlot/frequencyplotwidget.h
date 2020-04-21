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
    void adjustScrollbars() override;

public slots:
    void setWindowSize(int);
    void setWordSize(int);
    void setScale(int);

private:
    qint64 m_windowSize;
    qint64 m_wordSize;
    int m_scale;
};

#endif // FREQUENCYPLOTWIDGET_H
