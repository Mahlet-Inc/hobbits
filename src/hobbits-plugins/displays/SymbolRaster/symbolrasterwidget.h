#ifndef SYMBOLRASTERWIDGET_H
#define SYMBOLRASTERWIDGET_H

#include "displaybase.h"

class SymbolRasterWidget : public DisplayBase
{
    Q_OBJECT

public:
    SymbolRasterWidget(QSharedPointer<DisplayHandle> displayHandle, DisplayInterface *pluginRef);

    void paintEvent(QPaintEvent*) override;
    void mouseMoveEvent(QMouseEvent *event) override;

public slots:
    void setColorMapping(QList<QPair<QString, QColor>> mapping);
    void setScale(int scale);
    void setShowHeaders(bool showHeaders);

private:
    void prepareHeaders();
    QImage getSymbolMapImage(QSharedPointer<BitContainer> bitContainer, int x, int y, int w, int h) const;

    int m_scale;
    bool m_showFrameOffsets;
    bool m_showColumnOffsets;

    QPoint m_displayOffset;
    QSize m_headerFontSize;
    int m_symbolLength;
    QMap<quint64, QColor> m_colorMapping;

protected slots:
    void adjustScrollbars() override;

};

#endif // SYMBOLRASTERWIDGET_H
