#include "binaryview.h"

BinaryView::BinaryView() :
    m_displayWidget(nullptr),
    m_controlsWidget(nullptr)
{

}

DisplayInterface* BinaryView::createDefaultDisplay()
{
    return new BinaryView();
}

QString BinaryView::getName()
{
    return "Binary";
}

QWidget* BinaryView::getDisplayWidget(QSharedPointer<DisplayHandle> displayHandle)
{
    initialize(displayHandle);
    return m_displayWidget;
}

QWidget* BinaryView::getControlsWidget(QSharedPointer<DisplayHandle> displayHandle)
{
    initialize(displayHandle);
    return m_controlsWidget;
}

void BinaryView::initialize(QSharedPointer<DisplayHandle> displayHandle)
{
    if (!m_displayWidget) {
        m_displayWidget = new BinaryViewWidget(displayHandle, this);
        m_controlsWidget = new BinaryViewControls();

        connect(m_controlsWidget, SIGNAL(fontSizeChanged(int)), m_displayWidget, SLOT(setFontSize(int)));
        connect(m_controlsWidget, SIGNAL(columnGroupingChanged(int)), m_displayWidget, SLOT(setColumnGrouping(int)));
        connect(m_controlsWidget, SIGNAL(showHeadersChanged(bool)), m_displayWidget, SLOT(setShowHeaders(bool)));
    }
}
