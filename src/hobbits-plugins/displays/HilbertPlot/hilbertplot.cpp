#include "hilbertplot.h"

HilbertPlot::HilbertPlot() :
    m_displayWidget(nullptr),
    m_controlsWidget(nullptr)
{

}

DisplayInterface* HilbertPlot::createDefaultDisplay()
{
    return new HilbertPlot();
}

QString HilbertPlot::getName()
{
    return "Hilbert Plot";
}

QWidget* HilbertPlot::getDisplayWidget(QSharedPointer<DisplayHandle> displayHandle)
{
    initialize(displayHandle);
    return m_displayWidget;
}

QWidget* HilbertPlot::getControlsWidget(QSharedPointer<DisplayHandle> displayHandle)
{
    initialize(displayHandle);
    return m_controlsWidget;
}

void HilbertPlot::initialize(QSharedPointer<DisplayHandle> displayHandle)
{
    if (!m_displayWidget) {
        m_displayWidget = new HilbertPlotWidget(displayHandle, this);
        m_controlsWidget = new HilbertPlotControls();

        // make necessary connections here
    }
}
