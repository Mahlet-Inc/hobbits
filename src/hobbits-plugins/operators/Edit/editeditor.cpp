#include "editeditor.h"
#include "ui_editeditor.h"

EditEditor::EditEditor(QSharedPointer<ParameterDelegate> delegate):
    ui(new Ui::EditEditor()),
    m_paramHelper(new ParameterHelper(delegate))
{

    
    ui->setupUi(this);

    ui->sb_length->setMaximum(2147483647); //can this be set to the bitcontainer size?
    ui->sb_start->setMaximum(2147483647);

    ui->sb_length->setMinimum(0);
    ui->sb_start->setMinimum(0);
    //On spinbox value change, change the bits shown in pte_bits
    connect(ui->sb_start, SIGNAL(valueChanged(int)), this, SIGNAL(changed()));
    connect(ui->sb_length, SIGNAL(valueChanged(int)), this, SIGNAL(changed()));

    //On radio button value change
    connect(ui->rb_bit, SIGNAL(valueChanged(int)), this, SIGNAL(changed()));
    connect(ui->rb_hex, SIGNAL(valueChanged(int)), this, SIGNAL(changed()));
    connect(ui->rb_ascii, SIGNAL(valueChanged(int)), this, SIGNAL(changed()));


    m_paramHelper->addSpinBoxIntParameter("start", ui->sb_start);
    m_paramHelper->addSpinBoxIntParameter("length", ui->sb_length);

    m_paramHelper->addTextEditStringParameter("new_bits_in_range", ui->pte_bits);

    m_paramHelper->addParameter("edit_type", [this](QJsonValue value) {
        if (value.toString() == "bit") {
            ui->rb_bit->setChecked(true);
        }
        else if (value.toString() == "hex") {
            ui->rb_hex->setChecked(true);
        } else {
            ui->rb_ascii->setChecked(true);
        }
        return true;
    }, [this]() {
        if (ui->rb_bit->isChecked()) {
            return QJsonValue("bit");
        }
        else if (ui->rb_hex->isChecked()) {
            return QJsonValue("hex");
        } else {
            return QJsonValue("ascii");
        }
    });


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

EditEditor::~EditEditor()
{
    delete ui;
}

QString EditEditor::title()
{
    // TODO: Make this more descriptive
    return "Configure Edit";
}

Parameters EditEditor::parameters()
{
    return m_paramHelper->getParametersFromUi();
}

bool EditEditor::setParameters(const Parameters &parameters)
{
    return m_paramHelper->applyParametersToUi(parameters);
}

void EditEditor::previewBitsImpl(QSharedPointer<BitContainerPreview> container,
                                      QSharedPointer<PluginActionProgress> progress)
{
    // TODO: (Optional) Preview the currently active BitContainer (preprocess it, enrich it, etc)
}

void EditEditor::previewBitsUiImpl(QSharedPointer<BitContainerPreview> container)
{
    // TODO: (Optional) Update UI elements to account for preprocessing in previewBitsImpl and/or other metadata
}
