#include "hexview.h"

HexView::HexView() :
    m_displayWidget(nullptr),
    m_controlsWidget(nullptr)
{

}

DisplayInterface* HexView::createDefaultDisplay()
{
    return new HexView();
}

QString HexView::getName()
{
    return "Hex";
}

QWidget* HexView::getDisplayWidget(QSharedPointer<DisplayHandle> displayHandle)
{
    initialize(displayHandle);
    return m_displayWidget;
}

QWidget* HexView::getControlsWidget(QSharedPointer<DisplayHandle> displayHandle)
{
    initialize(displayHandle);
    return m_controlsWidget;
}

void HexView::initialize(QSharedPointer<DisplayHandle> displayHandle)
{
    if (!m_displayWidget) {
        m_displayWidget = new HexViewWidget(displayHandle, this);
        m_controlsWidget = new HexViewControls();

        connect(m_controlsWidget, SIGNAL(fontSizeChanged(int)), m_displayWidget, SLOT(setFontSize(int)));
        connect(m_controlsWidget, SIGNAL(columnGroupingChanged(int)), m_displayWidget, SLOT(setColumnGrouping(int)));
        connect(m_controlsWidget, SIGNAL(showHeadersChanged(bool)), m_displayWidget, SLOT(setShowHeaders(bool)));
    }
}
