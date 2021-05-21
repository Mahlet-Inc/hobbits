#include "simpleparametereditor.h"
#include "ui_simpleparametereditor.h"
#include <QLineEdit>
#include <QSpinBox>
#include <QDoubleSpinBox>
#include <QCheckBox>
#include <QLabel>

SimpleParameterEditor::SimpleParameterEditor(QSharedPointer<ParameterDelegate> delegate, QString title) :
    ui(new Ui::SimpleParameterEditor),
    m_stateHelper(new ParameterHelper(delegate)),
    m_title(title)
{
    ui->setupUi(this);

    for (auto param: delegate->parameterInfos()) {
        if (!param.possibleValues.isEmpty()) {
            auto comboBox = new QComboBox();
            for (auto value : param.possibleValues) {
                comboBox->addItem(value.toVariant().toString(), value.toVariant());
            }
            ui->formLayout->addRow(param.name, comboBox);
            m_stateHelper->addComboBoxParameter(param.name, comboBox);
            connect(comboBox, SIGNAL(currentIndexChanged(int)), this, SIGNAL(changed()));
        }
        else if (param.type == ParameterDelegate::ParameterType::String) {
            auto lineEdit = new QLineEdit();
            ui->formLayout->addRow(param.name, lineEdit);
            m_stateHelper->addLineEditStringParameter(param.name, lineEdit);
            connect(lineEdit, SIGNAL(textChanged(QString)), this, SIGNAL(changed()));
        }
        else if (param.type == ParameterDelegate::ParameterType::Decimal) {
            auto spinBox = new QDoubleSpinBox();
            spinBox->setRange(0.0, 9000000.0);
            if (param.ranges.size() == 1) {
                spinBox->setRange(param.ranges.first().first, param.ranges.first().second);
            }
            ui->formLayout->addRow(param.name, spinBox);
            m_stateHelper->addSpinBoxDoubleParameter(param.name, spinBox);
            connect(spinBox, SIGNAL(valueChanged(double)), this, SIGNAL(changed()));
        }
        else if (param.type == ParameterDelegate::ParameterType::Integer) {
            auto spinBox = new QSpinBox();
            spinBox->setRange(0, INT_MAX);
            if (param.ranges.size() == 1) {
                spinBox->setRange(param.ranges.first().first, param.ranges.first().second);
            }
            ui->formLayout->addRow(param.name, spinBox);
            m_stateHelper->addSpinBoxIntParameter(param.name, spinBox);
            connect(spinBox, SIGNAL(valueChanged(int)), this, SIGNAL(changed()));
        }
        else if (param.type == ParameterDelegate::ParameterType::Boolean) {
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

bool SimpleParameterEditor::setParameters(const Parameters &parameters)
{
    return m_stateHelper->applyParametersToUi(parameters);
}

Parameters SimpleParameterEditor::parameters()
{
    return m_stateHelper->getParametersFromUi();
}
