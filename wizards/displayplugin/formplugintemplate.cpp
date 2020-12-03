#include "%{FormHeaderFileName}"
#include "ui_%{FormHeaderFileName}"

%{FormClassName}::%{FormClassName}(QSharedPointer<ParameterDelegate> delegate):
    ui(new Ui::%{FormClassName}()),
    m_paramHelper(new ParameterHelper(delegate))
{
    ui->setupUi(this);

    // TODO: Correlate parameters in given delegate to form fields
    // Ex 1.
    // m_paramHelper->addSpinBoxIntParameter("myparametername", ui->spinBox);
    // Ex 2.
    // m_paramHelper->addParameter("otherparameter", [this](QJsonValue value) {
    //     // set the value in the editor
    //     ui->spinBox->setValue(value.toInt());
    //     return true; // return false if unable to set the value in the editor
    // }, [this]() {
    //     // get the QJsonValue from the editor
    //     return QJsonValue(ui->spinBox->value());
    // });
}

%{FormClassName}::~%{FormClassName}()
{
    delete ui;
}

QString %{FormClassName}::title()
{
    return "Configure %{ClassName} Display";
}

QJsonObject %{FormClassName}::parameters()
{
    return m_paramHelper->getParametersFromUi();
}

bool %{FormClassName}::setParameters(QJsonObject parameters)
{
    return m_paramHelper->applyParametersToUi(parameters);
}

void %{FormClassName}::previewBitsImpl(QSharedPointer<BitContainerPreview> container,
                                      QSharedPointer<PluginActionProgress> progress)
{
    // TODO: (Optional) Preview the currently active BitContainer (preprocess it, enrich it, etc)
}

void %{FormClassName}::previewBitsUiImpl(QSharedPointer<BitContainerPreview> container)
{
    // TODO: (Optional) Update UI elements to account for preprocessing in previewBitsImpl and/or other metadata
}