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

QString BinaryView::name()
{
    return "Binary";
}

QString BinaryView::description()
{
    return "Display data as 1s and 0s";
}

QStringList BinaryView::tags()
{
    return {"Generic"};
}

QWidget* BinaryView::display(QSharedPointer<DisplayHandle> displayHandle)
{
    initialize(displayHandle);
    return m_displayWidget;
}

QWidget* BinaryView::controls(QSharedPointer<DisplayHandle> displayHandle)
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
