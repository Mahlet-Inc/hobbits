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

QString HilbertPlot::name()
{
    return "Hilbert Plot";
}

QString HilbertPlot::description()
{
    return "Displays byte-based pixels in a fractal pattern to highlight chunks in heterogeneous data";
}

QStringList HilbertPlot::tags()
{
    return {"Generic"};
}

QWidget* HilbertPlot::display(QSharedPointer<DisplayHandle> displayHandle)
{
    initialize(displayHandle);
    return m_displayWidget;
}

QWidget* HilbertPlot::controls(QSharedPointer<DisplayHandle> displayHandle)
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
