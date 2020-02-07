#ifndef BYTERASTERWIDGET_H
#define BYTERASTERWIDGET_H

#include "displaybase.h"

class ByteRasterWidget : public DisplayBase
{
    Q_OBJECT

public:
    ByteRasterWidget(QSharedPointer<DisplayHandle> displayHandle, DisplayInterface *pluginRef);

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

#endif // BYTERASTERWIDGET_H
