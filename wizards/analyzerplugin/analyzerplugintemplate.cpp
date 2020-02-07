#include "%{HeaderFileName}"
#include "ui_%{HeaderFileName}"

%{ClassName}::%{ClassName}() :
    ui(new Ui::%{ClassName}())
{

}

%{ClassName}::~%{ClassName}()
{
    delete ui;
}

AnalyzerInterface* %{ClassName}::createDefaultAnalyzer()
{
    return new %{ClassName}();
}

QString %{ClassName}::getName()
{
    return "%{ClassName}";
}

void %{ClassName}::provideCallback(QSharedPointer<PluginCallback> pluginCallback)
{
    // The plugin callback allows the self-triggering of analyzeBits
    m_pluginCallback = pluginCallback;
}

void %{ClassName}::applyToWidget(QWidget *widget)
{
    ui->setupUi(widget);
}

bool %{ClassName}::canRecallPluginState(const QJsonObject &pluginState)
{
    // If pluginState does not have required fields, return false
    if(pluginState.isEmpty() == true) {
        return false;
    }

    return true;
}

bool %{ClassName}::setPluginStateInUi(const QJsonObject &pluginState)
{
    if (!canRecallPluginState(pluginState)) {
        return false;
    }

    // Set the UI fields based on the plugin state

    return true;
}

QJsonObject %{ClassName}::getStateFromUi()
{
    QJsonObject pluginState;

    // Pull data from the input fields and input them into pluginState

    return pluginState;
}

void %{ClassName}::previewBits(QSharedPointer<BitContainerPreview> container)
{
    // Prepare operations for the current bit container
}

QSharedPointer<const AnalyzerResult> %{ClassName}::analyzeBits(
        QSharedPointer<const BitContainer> container,
        const QJsonObject &recallablePluginState,
        QSharedPointer<ActionProgress> progressTracker)
{
    QSharedPointer<AnalyzerResult> result(new AnalyzerResult());

    //Perform analysis

    return result;
}
