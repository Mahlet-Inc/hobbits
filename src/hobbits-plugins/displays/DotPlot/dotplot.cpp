#include "dotplot.h"

DotPlot::DotPlot() :
    m_displayWidget(nullptr),
    m_controlsWidget(nullptr)
{

}

DisplayInterface* DotPlot::createDefaultDisplay()
{
    return new DotPlot();
}

QString DotPlot::getName()
{
    return "Dot Plot";
}

QWidget* DotPlot::getDisplayWidget(QSharedPointer<DisplayHandle> displayHandle)
{
    initialize(displayHandle);
    return m_displayWidget;
}

QWidget* DotPlot::getControlsWidget(QSharedPointer<DisplayHandle> displayHandle)
{
    initialize(displayHandle);
    return m_controlsWidget;
}

void DotPlot::initialize(QSharedPointer<DisplayHandle> displayHandle)
{
    if (!m_displayWidget) {
        m_displayWidget = new DotPlotWidget(displayHandle, this);
        m_controlsWidget = new DotPlotControls();

        connect(m_controlsWidget,
                &DotPlotControls::newWordSize,
                m_displayWidget,
                &DotPlotWidget::setWordSize);
        connect(m_controlsWidget,
                &DotPlotControls::newWindowSize,
                m_displayWidget,
                &DotPlotWidget::setWindowSize);
        connect(m_controlsWidget,
                &DotPlotControls::newScale,
                m_displayWidget,
                &DotPlotWidget::setScale);
    }
}
