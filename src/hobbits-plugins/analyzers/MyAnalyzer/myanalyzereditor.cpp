#include "myanalyzereditor.h"
#include "ui_myanalyzereditor.h"

MyAnalyzerEditor::MyAnalyzerEditor(QSharedPointer<ParameterDelegate> delegate):
    ui(new Ui::MyAnalyzerEditor()),
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

MyAnalyzerEditor::~MyAnalyzerEditor()
{
    delete ui;
}

QString MyAnalyzerEditor::title()
{
    // TODO: Make this more descriptive
    return "Configure MyAnalyzer";
}

QJsonObject MyAnalyzerEditor::parameters()
{
    return m_paramHelper->getParametersFromUi();
}

bool MyAnalyzerEditor::setParameters(QJsonObject parameters)
{
    return m_paramHelper->applyParametersToUi(parameters);
}

void MyAnalyzerEditor::previewBitsImpl(QSharedPointer<BitContainerPreview> container,
                                      QSharedPointer<PluginActionProgress> progress)
{
    // TODO: (Optional) Preview the currently active BitContainer (preprocess it, enrich it, etc)
}

void MyAnalyzerEditor::previewBitsUiImpl(QSharedPointer<BitContainerPreview> container)
{
    // TODO: (Optional) Update UI elements to account for preprocessing in previewBitsImpl and/or other metadata
}
