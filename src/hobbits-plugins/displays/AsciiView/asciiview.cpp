#include "asciiview.h"

AsciiView::AsciiView() :
    m_displayWidget(nullptr),
    m_controlsWidget(nullptr)
{

}

DisplayInterface* AsciiView::createDefaultDisplay()
{
    return new AsciiView();
}

QString AsciiView::getName()
{
    return "ASCII";
}

QWidget* AsciiView::getDisplayWidget(QSharedPointer<DisplayHandle> displayHandle)
{
    initialize(displayHandle);
    return m_displayWidget;
}

QWidget* AsciiView::getControlsWidget(QSharedPointer<DisplayHandle> displayHandle)
{
    initialize(displayHandle);
    return m_controlsWidget;
}

void AsciiView::initialize(QSharedPointer<DisplayHandle> displayHandle)
{
    if (!m_displayWidget) {
        m_displayWidget = new AsciiViewWidget(displayHandle, this);
        m_controlsWidget = new AsciiViewControls();

        connect(m_controlsWidget, SIGNAL(fontSizeChanged(int)), m_displayWidget, SLOT(setFontSize(int)));
        connect(m_controlsWidget, SIGNAL(columnGroupingChanged(int)), m_displayWidget, SLOT(setColumnGrouping(int)));
        connect(m_controlsWidget, SIGNAL(showHeadersChanged(bool)), m_displayWidget, SLOT(setShowHeaders(bool)));
    }
}
