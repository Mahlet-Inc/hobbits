#include "{{cookiecutter.editor_class|lower}}.h"
#include "ui_{{cookiecutter.editor_class|lower}}.h"

{{cookiecutter.editor_class}}::{{cookiecutter.editor_class}}(QSharedPointer<ParameterDelegate> delegate):
    ui(new Ui::{{cookiecutter.editor_class}}()),
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

{{cookiecutter.editor_class}}::~{{cookiecutter.editor_class}}()
{
    delete ui;
}

QString {{cookiecutter.editor_class}}::title()
{
    return "Configure {{cookiecutter.class}} Display";
}

QJsonObject {{cookiecutter.editor_class}}::parameters()
{
    return m_paramHelper->getParametersFromUi();
}

bool {{cookiecutter.editor_class}}::setParameters(QJsonObject parameters)
{
    return m_paramHelper->applyParametersToUi(parameters);
}

void {{cookiecutter.editor_class}}::previewBitsImpl(QSharedPointer<BitContainerPreview> container,
                                      QSharedPointer<PluginActionProgress> progress)
{
    // TODO: (Optional) Preview the currently active BitContainer (preprocess it, enrich it, etc)
}

void {{cookiecutter.editor_class}}::previewBitsUiImpl(QSharedPointer<BitContainerPreview> container)
{
    // TODO: (Optional) Update UI elements to account for preprocessing in previewBitsImpl and/or other metadata
}
