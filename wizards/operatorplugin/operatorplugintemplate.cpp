#include "%{JS: '%{HeaderFileName}'.toLowerCase()}"
#include "ui_%{JS: '%{HeaderFileName}'.toLowerCase()}"
#include <QObject>

%{ClassName}::%{ClassName}() :
    ui(new Ui::%{ClassName}()),
    m_stateHelper(new PluginStateHelper())
{
    // The PluginStateHelper can be used to simplify the setting, getting, and validation
    // of the plugin state. It can simplify the canRecallPluginState, setPluginStateInUi,
    // and getStateFromUi methods.
    // Using PluginStateHelper is completely optional - you can just implement those
    // methods manually and remove m_stateHelper if desired.
    m_stateHelper->addParameter("plugin_parameter", QJsonValue::String, [this](QJsonValue value) {
        // Take the value and apply it to the ui
        // e.g. ui->parameterLineEdit->setText(value.toString());
        // Return true if it was able to be set in the ui
        return true;
    }, [this]() {
        // Get the value from the ui
        // e.g. return QJsonValue(ui->parameterLineEdit->text())
        return QJsonValue("");
    });
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
    return m_stateHelper->validatePluginState(pluginState);
}

bool %{ClassName}::setPluginStateInUi(const QJsonObject &pluginState)
{
    return m_stateHelper->applyPluginStateToUi(pluginState);
}

QJsonObject %{ClassName}::getStateFromUi()
{
    return m_stateHelper->getPluginStateFromUi();
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
