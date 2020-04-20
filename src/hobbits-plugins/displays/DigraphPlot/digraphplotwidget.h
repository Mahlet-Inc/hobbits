#ifndef DIGRAPHPLOTWIDGET_H
#define DIGRAPHPLOTWIDGET_H

#include "displaybase.h"

class DigraphPlotWidget : public DisplayBase
{
    Q_OBJECT

public:
    DigraphPlotWidget(
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

#endif // DIGRAPHPLOTWIDGET_H
