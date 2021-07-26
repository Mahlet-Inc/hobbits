#include "usbdeviceimporteditor.h"
#include "ui_usbdeviceimporteditor.h"

USBDeviceImportEditor::USBDeviceImportEditor(QSharedPointer<ParameterDelegate> delegate):
    ui(new Ui::USBDeviceImportEditor()),
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

USBDeviceImportEditor::~USBDeviceImportEditor()
{
    delete ui;
}

QString USBDeviceImportEditor::title()
{
    // TODO: Make this more descriptive
    return "Configure USBDevice";
}

Parameters USBDeviceImportEditor::parameters()
{
    return m_paramHelper->getParametersFromUi();
}

bool USBDeviceImportEditor::setParameters(const Parameters &parameters)
{
    return m_paramHelper->applyParametersToUi(parameters);
}

void USBDeviceImportEditor::previewBitsImpl(QSharedPointer<BitContainerPreview> container,
                                      QSharedPointer<PluginActionProgress> progress)
{
    // TODO: (Optional) Preview the currently active BitContainer (preprocess it, enrich it, etc)
}

void USBDeviceImportEditor::previewBitsUiImpl(QSharedPointer<BitContainerPreview> container)
{
    // TODO: (Optional) Update UI elements to account for preprocessing in previewBitsImpl and/or other metadata
}
