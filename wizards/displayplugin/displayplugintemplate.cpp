#include "%{PluginHeaderFileName}"

%{PluginName}::%{PluginName}() :
    m_displayWidget(nullptr),
    m_controlsWidget(nullptr)
{

}

DisplayInterface* %{PluginName}::createDefaultDisplay()
{
    return new %{PluginName}();
}

QString %{PluginName}::getName()
{
    return "%{PluginName}";
}

QWidget* %{PluginName}::getDisplayWidget(QSharedPointer<DisplayHandle> displayHandle)
{
    initialize(displayHandle);
    return m_displayWidget;
}

QWidget* %{PluginName}::getControlsWidget(QSharedPointer<DisplayHandle> displayHandle)
{
    initialize(displayHandle);
    return m_controlsWidget;
}

void %{PluginName}::initialize(QSharedPointer<DisplayHandle> displayHandle)
{
    if (!m_displayWidget) {
        m_displayWidget = new %{DisplayWidgetName}(displayHandle, this);
        m_controlsWidget = new %{ControlWidgetName}();

        // make necessary connections here
    }
}
