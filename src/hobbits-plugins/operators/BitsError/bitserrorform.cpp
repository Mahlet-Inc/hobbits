#include "bitserrorform.h"
#include "ui_bitserrorform.h"

BitsErrorForm::BitsErrorForm(QSharedPointer<ParameterDelegate> delegate):
    ui(new Ui::BitsErrorForm()),
    m_stateHelper(new ParameterHelper(delegate))
{
    ui->setupUi(this);

    m_stateHelper->addParameter("error_coeff", [this](QJsonValue value) {
        ui->coeffInput->setText(QString("%1").arg(value.toDouble()));
        return true;
    }, [this]() {
        return QJsonValue(ui->coeffInput->text().toDouble());
    });

    m_stateHelper->addSpinBoxIntParameter("error_exp", ui->expInput);

    m_stateHelper->addParameter("error_type", [this](QJsonValue value) {
        if (value.toString() == "periodic") {
            ui->periodicOpt->setChecked(true);
        }
        else {
            ui->gaussianOpt->setChecked(true);
        }
        return true;
    }, [this]() {
        if (ui->periodicOpt->isChecked()) {
            return QJsonValue("periodic");
        }
        else {
            return QJsonValue("gaussian");
        }
    });
}

BitsErrorForm::~BitsErrorForm()
{
    delete ui;
}

QString BitsErrorForm::title()
{
    return "Configure Take Skip";
}

QJsonObject BitsErrorForm::parameters()
{
    return m_stateHelper->getParametersFromUi();
}

bool BitsErrorForm::setParameters(QJsonObject parameters)
{
    return m_stateHelper->applyParametersToUi(parameters);
}

