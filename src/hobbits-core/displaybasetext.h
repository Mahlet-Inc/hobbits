#ifndef DISPLAYBASETEXT_H
#define DISPLAYBASETEXT_H

#include "displaybase.h"
#include <QWidget>

#include "hobbits-core_global.h"

class HOBBITSCORESHARED_EXPORT DisplayBaseText : public DisplayBase
{
    Q_OBJECT

public:
    DisplayBaseText(QSharedPointer<DisplayHandle> displayHandle, DisplayInterface *pluginRef, int columnGrouping);

    void paintEvent(QPaintEvent*);
    void mouseMoveEvent(QMouseEvent *event);

    virtual QString getDisplayChars(Frame frame, int offset) = 0;
    virtual int bitsPerChar() = 0;

public slots:
    void setFontSize(int fontSize);
    void setColumnGrouping(int columnGrouping);
    void setShowHeaders(bool showHeaders);

private:
    void prepareHeaders();

    int m_fontSize;
    int m_columnGrouping;
    bool m_showFrameOffsets;
    bool m_showColumnOffsets;

    QPoint m_displayOffset;

    int m_fontWidth;
    int m_fontHeight;
    int m_frameHeight;

protected slots:
    void adjustScrollbars() override;

};

#endif // DISPLAYBASETEXT_H
