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

QString %{PluginName}::name()
{
    return "%{PluginName}";
}

QString %{PluginName}::description()
{
    // TODO: add description
    return "%{PluginName}";
}

QStringList %{PluginName}::tags()
{
    // TODO: make tags relevant
    return {"Generic"};
}

QWidget* %{PluginName}::display(QSharedPointer<DisplayHandle> displayHandle)
{
    initialize(displayHandle);
    return m_displayWidget;
}

QWidget* %{PluginName}::controls(QSharedPointer<DisplayHandle> displayHandle)
{
    initialize(displayHandle);
    return m_controlsWidget;
}

void %{PluginName}::initialize(QSharedPointer<DisplayHandle> displayHandle)
{
    if (!m_displayWidget) {
        m_displayWidget = new %{DisplayWidgetName}(displayHandle, this);
        m_controlsWidget = new %{ControlWidgetName}();

        // TODO: make necessary connections here
    }
}
