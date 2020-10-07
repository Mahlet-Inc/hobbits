#include "frequencyplot.h"

FrequencyPlot::FrequencyPlot() :
    m_displayWidget(nullptr),
    m_controlsWidget(nullptr)
{

}

DisplayInterface* FrequencyPlot::createDefaultDisplay()
{
    return new FrequencyPlot();
}

QString FrequencyPlot::name()
{
    return "Frequency Plot";
}

QString FrequencyPlot::description()
{
    return "Displays a histogram where each bar shows the frequency of an n-bit word across the given sample window";
}

QStringList FrequencyPlot::tags()
{
    return {"Generic"};
}

QWidget* FrequencyPlot::display(QSharedPointer<DisplayHandle> displayHandle)
{
    initialize(displayHandle);
    return m_displayWidget;
}

QWidget* FrequencyPlot::controls(QSharedPointer<DisplayHandle> displayHandle)
{
    initialize(displayHandle);
    return m_controlsWidget;
}

void FrequencyPlot::initialize(QSharedPointer<DisplayHandle> displayHandle)
{
    if (!m_displayWidget) {
        m_displayWidget = new FrequencyPlotWidget(displayHandle, this);
        m_controlsWidget = new FrequencyPlotControls();

        connect(m_controlsWidget,
                &FrequencyPlotControls::newWordSize,
                m_displayWidget,
                &FrequencyPlotWidget::setWordSize);
        connect(m_controlsWidget,
                &FrequencyPlotControls::newWindowSize,
                m_displayWidget,
                &FrequencyPlotWidget::setWindowSize);
        connect(m_controlsWidget,
                &FrequencyPlotControls::newScale,
                m_displayWidget,
                &FrequencyPlotWidget::setScale);
    }
}
