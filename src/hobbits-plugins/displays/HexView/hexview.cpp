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

QString HexView::name()
{
    return "Hex";
}

QString HexView::description()
{
    return "Displays data as a hexadecimal string";
}

QStringList HexView::tags()
{
    return {"Generic"};
}

QWidget* HexView::display(QSharedPointer<DisplayHandle> displayHandle)
{
    initialize(displayHandle);
    return m_displayWidget;
}

QWidget* HexView::controls(QSharedPointer<DisplayHandle> displayHandle)
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
