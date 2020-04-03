#include "%{JS: '%{HeaderFileName}'.toLowerCase()}"
#include "ui_%{JS: '%{HeaderFileName}'.toLowerCase()}"
#include <QObject>

%{ClassName}::%{ClassName}() :
    ui(new Ui::%{ClassName}())
{

}

OperatorInterface* %{ClassName}::createDefaultOperator()
{
    return new %{ClassName}();
}

//Return name of operator
QString %{ClassName}::getName()
{
    return "%{ClassName}";
}

void %{ClassName}::provideCallback(QSharedPointer<PluginCallback> pluginCallback)
{
    // the plugin callback allows the self-triggering of operateOnContainers
    m_pluginCallback = pluginCallback;
}

void %{ClassName}::applyToWidget(QWidget *widget)
{
    ui->setupUi(widget);
}

bool %{ClassName}::canRecallPluginState(const QJsonObject &pluginState)
{
    //if pluginState does not have required fields, return false
    if(pluginState.isEmpty()==true){
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

    //Pull data from the input fields and input them into pluginState

    return pluginState;
}

int %{ClassName}::getMinInputContainers(const QJsonObject &pluginState)
{
    Q_UNUSED(pluginState)
    return 1;
}

int %{ClassName}::getMaxInputContainers(const QJsonObject &pluginState)
{
    Q_UNUSED(pluginState)
    return 1;
}

QSharedPointer<const OperatorResult> %{ClassName}::operateOnContainers(
    QList<QSharedPointer<const BitContainer> > inputContainers,
    const QJsonObject &recallablePluginState,
    QSharedPointer<ActionProgress> progressTracker)
{     
    QSharedPointer<OperatorResult> result(new OperatorResult());
    //Perform bit operations here

    return OperatorResult::error("Plugin operation is not implemented!");
    //return OperatorResult::result({outputContainer}, recallablePluginState);
}

void %{ClassName}::previewBits(QSharedPointer<BitContainerPreview> container)
{
    Q_UNUSED(container)
    // optionally use the current container to prepare the UI or something
}
