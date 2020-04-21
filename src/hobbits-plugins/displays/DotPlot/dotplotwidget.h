#ifndef DOTPLOTWIDGET_H
#define DOTPLOTWIDGET_H

#include "displaybase.h"

class DotPlotWidget : public DisplayBase
{
    Q_OBJECT

public:
    DotPlotWidget(
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
    int m_windowSize;
    int m_wordSize;
    int m_scale;
};

#endif // DOTPLOTWIDGET_H
