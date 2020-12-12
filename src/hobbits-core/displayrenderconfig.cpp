#include "displayrenderconfig.h"

DisplayRenderConfig::DisplayRenderConfig() :
    m_asynchronous(false),
    m_hideBitOffsetControls(false),
    m_hideFrameOffsetControls(false),
    m_fullRedrawTriggers(0),
    m_overlayRedrawTriggers(0)
{

}

bool DisplayRenderConfig::asynchronous() const
{
    return m_asynchronous;
}
void DisplayRenderConfig::setAsynchronous(bool asynchronous)
{
    m_asynchronous = asynchronous;
}

bool DisplayRenderConfig::hideFrameOffsetControls() const
{
    return m_hideFrameOffsetControls;
}

void DisplayRenderConfig::setHideFrameOffsetControls(bool hideFrameOffsetControls)
{
    m_hideFrameOffsetControls = hideFrameOffsetControls;
}

bool DisplayRenderConfig::hideBitOffsetControls() const
{
    return m_hideBitOffsetControls;
}

void DisplayRenderConfig::setHideBitOffsetControls(bool hideBitOffsetControls)
{
    m_hideBitOffsetControls = hideBitOffsetControls;
}

int DisplayRenderConfig::fullRedrawTriggers() const
{
    return m_fullRedrawTriggers;
}

void DisplayRenderConfig::setFullRedrawTriggers(int fullRedrawTriggers)
{
    m_fullRedrawTriggers = fullRedrawTriggers;
}

int DisplayRenderConfig::overlayRedrawTriggers() const
{
    return m_overlayRedrawTriggers;
}

void DisplayRenderConfig::setOverlayRedrawTriggers(int overlayRedrawTriggers)
{
    m_overlayRedrawTriggers = overlayRedrawTriggers;
}
