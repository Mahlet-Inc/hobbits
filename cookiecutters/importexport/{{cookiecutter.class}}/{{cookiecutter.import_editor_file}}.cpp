#include "{{cookiecutter.import_editor|lower}}.h"
#include "ui_{{cookiecutter.import_editor|lower}}.h"

{{cookiecutter.import_editor}}::{{cookiecutter.import_editor}}(QSharedPointer<ParameterDelegate> delegate):
    ui(new Ui::{{cookiecutter.import_editor}}()),
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

{{cookiecutter.import_editor}}::~{{cookiecutter.import_editor}}()
{
    delete ui;
}

QString {{cookiecutter.import_editor}}::title()
{
    // TODO: Make this more descriptive
    return "Configure {{cookiecutter.class}}";
}

Parameter {{cookiecutter.import_editor}}::parameters()
{
    return m_paramHelper->getParametersFromUi();
}

bool {{cookiecutter.import_editor}}::setParameters(const Parameters &parameters)
{
    return m_paramHelper->applyParametersToUi(parameters);
}

void {{cookiecutter.import_editor}}::previewBitsImpl(QSharedPointer<BitContainerPreview> container,
                                      QSharedPointer<PluginActionProgress> progress)
{
    // TODO: (Optional) Preview the currently active BitContainer (preprocess it, enrich it, etc)
}

void {{cookiecutter.import_editor}}::previewBitsUiImpl(QSharedPointer<BitContainerPreview> container)
{
    // TODO: (Optional) Update UI elements to account for preprocessing in previewBitsImpl and/or other metadata
}
