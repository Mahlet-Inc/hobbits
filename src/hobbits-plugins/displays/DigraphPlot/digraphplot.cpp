#include "digraphplot.h"

DigraphPlot::DigraphPlot() :
    m_displayWidget(nullptr),
    m_controlsWidget(nullptr)
{

}

DisplayInterface* DigraphPlot::createDefaultDisplay()
{
    return new DigraphPlot();
}

QString DigraphPlot::getName()
{
    return "Digraph Plot";
}

QWidget* DigraphPlot::getDisplayWidget(QSharedPointer<DisplayHandle> displayHandle)
{
    initialize(displayHandle);
    return m_displayWidget;
}

QWidget* DigraphPlot::getControlsWidget(QSharedPointer<DisplayHandle> displayHandle)
{
    initialize(displayHandle);
    return m_controlsWidget;
}

void DigraphPlot::initialize(QSharedPointer<DisplayHandle> displayHandle)
{
    if (!m_displayWidget) {
        m_displayWidget = new DigraphPlotWidget(displayHandle, this);
        m_controlsWidget = new DigraphPlotControls();

        connect(m_controlsWidget,
                &DigraphPlotControls::newWordSize,
                m_displayWidget,
                &DigraphPlotWidget::setWordSize);
        connect(m_controlsWidget,
                &DigraphPlotControls::newWindowSize,
                m_displayWidget,
                &DigraphPlotWidget::setWindowSize);
        connect(m_controlsWidget,
                &DigraphPlotControls::newScale,
                m_displayWidget,
                &DigraphPlotWidget::setScale);
    }
}
