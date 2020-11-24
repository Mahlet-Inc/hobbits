#ifndef DISPLAYRENDERCONFIG_H
#define DISPLAYRENDERCONFIG_H

#include <QObject>

class DisplayRenderConfig : public QObject
{
    Q_OBJECT
public:
    enum RedrawTriggers {
        NewBitOffset = 0x01,
        NewFrameOffset = 0x02,
        NewBitHover = 0x04,
        NewMouseHover = 0x08
    };

    DisplayRenderConfig();

    bool asynchronous() const;
    void setAsynchronous(bool asynchronous);

    bool hideFrameOffsetControls() const;
    void setHideFrameOffsetControls(bool hideFrameOffsetControls);

    bool hideBitOffsetControls() const;
    void setHideBitOffsetControls(bool hideBitOffsetControls);

    int fullRedrawTriggers() const;
    void setFullRedrawTriggers(int fullRedrawTriggers);

    int overlayRedrawTriggers() const;
    void setOverlayRedrawTriggers(int overlayRedrawTriggers);

private:
    bool m_asynchronous;
    bool m_hideBitOffsetControls;
    bool m_hideFrameOffsetControls;
    int m_fullRedrawTriggers;
    int m_overlayRedrawTriggers;
};

#endif // DISPLAYRENDERCONFIG_H
