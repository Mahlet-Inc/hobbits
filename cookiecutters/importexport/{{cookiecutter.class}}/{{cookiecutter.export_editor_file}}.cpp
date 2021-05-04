#include "{{cookiecutter.export_editor|lower}}.h"
#include "ui_{{cookiecutter.export_editor|lower}}.h"

{{cookiecutter.export_editor}}::{{cookiecutter.export_editor}}(QSharedPointer<ParameterDelegate> delegate):
    ui(new Ui::{{cookiecutter.export_editor}}()),
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

{{cookiecutter.export_editor}}::~{{cookiecutter.export_editor}}()
{
    delete ui;
}

QString {{cookiecutter.export_editor}}::title()
{
    // TODO: Make this more descriptive
    return "Configure {{cookiecutter.class}}";
}

QJsonObject {{cookiecutter.export_editor}}::parameters()
{
    return m_paramHelper->getParametersFromUi();
}

bool {{cookiecutter.export_editor}}::setParameters(QJsonObject parameters)
{
    return m_paramHelper->applyParametersToUi(parameters);
}

void {{cookiecutter.export_editor}}::previewBitsImpl(QSharedPointer<BitContainerPreview> container,
                                      QSharedPointer<PluginActionProgress> progress)
{
    // TODO: (Optional) Preview the currently active BitContainer (preprocess it, enrich it, etc)
}

void {{cookiecutter.export_editor}}::previewBitsUiImpl(QSharedPointer<BitContainerPreview> container)
{
    // TODO: (Optional) Update UI elements to account for preprocessing in previewBitsImpl and/or other metadata
}
