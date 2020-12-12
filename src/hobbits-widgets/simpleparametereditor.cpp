#include "simpleparametereditor.h"
#include "ui_simpleparametereditor.h"
#include <QLineEdit>
#include <QSpinBox>
#include <QCheckBox>
#include <QLabel>

SimpleParameterEditor::SimpleParameterEditor(QSharedPointer<ParameterDelegate> delegate, QString title) :
    ui(new Ui::SimpleParameterEditor),
    m_stateHelper(new ParameterHelper(delegate)),
    m_title(title)
{
    ui->setupUi(this);

    for (auto param: delegate->parameterInfos()) {
        if (param.type == QJsonValue::String) {
            auto lineEdit = new QLineEdit();
            ui->formLayout->addRow(param.name, lineEdit);
            m_stateHelper->addLineEditStringParameter(param.name, lineEdit);
            connect(lineEdit, SIGNAL(textChanged(QString)), this, SIGNAL(changed()));
        }
        else if (param.type == QJsonValue::Double) {
            auto spinBox = new QSpinBox();
            spinBox->setRange(0, 0x7fffffff);
            if (param.hasIntLimits) {
                spinBox->setRange(param.intMin, param.intMax);
            }
            ui->formLayout->addRow(param.name, spinBox);
            m_stateHelper->addSpinBoxIntParameter(param.name, spinBox);
            connect(spinBox, SIGNAL(valueChanged(int)), this, SIGNAL(changed()));
        }
        else if (param.type == QJsonValue::Bool) {
            auto checkBox = new QCheckBox(param.name);
            ui->formLayout->addRow("", checkBox);
            m_stateHelper->addCheckBoxBoolParameter(param.name, checkBox);
            connect(checkBox, SIGNAL(toggled(bool)), this, SIGNAL(changed()));
        }
    }
}

SimpleParameterEditor::~SimpleParameterEditor()
{
    delete ui;
}

QString SimpleParameterEditor::title()
{
    return m_title;
}

bool SimpleParameterEditor::setParameters(QJsonObject parameters)
{
    return m_stateHelper->applyParametersToUi(parameters);
}

QJsonObject SimpleParameterEditor::parameters()
{
    return m_stateHelper->getParametersFromUi();
}
