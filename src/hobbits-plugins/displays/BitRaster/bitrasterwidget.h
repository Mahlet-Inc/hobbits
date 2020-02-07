#ifndef BITRASTERWIDGET_H
#define BITRASTERWIDGET_H

#include "displaybase.h"

class BitRasterWidget : public DisplayBase
{
    Q_OBJECT

public:
    BitRasterWidget(QSharedPointer<DisplayHandle> displayHandle, DisplayInterface *pluginRef);

    void paintEvent(QPaintEvent*) override;
    void mouseMoveEvent(QMouseEvent *event) override;

public slots:
    void setScale(int scale);
    void setShowHeaders(bool showHeaders);

private:
    void prepareHeaders();

    int m_scale;
    bool m_showFrameOffsets;
    bool m_showColumnOffsets;

    QPoint m_displayOffset;
    QSize m_headerFontSize;

protected slots:
    void adjustScrollbars() override;

};

#endif // BITRASTERWIDGET_H
