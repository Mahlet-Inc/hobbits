#include "lfsrform.h"
#include "ui_lfsrform.h"
#include "mathparser.h"
#include <QMessageBox>

LfsrForm::LfsrForm(QSharedPointer<ParameterDelegate> delegate) :
    ui(new Ui::LfsrForm()),
    m_stateHelper(new ParameterHelper(delegate))
{
    ui->setupUi(this);

    m_stateHelper->addLineEditStringParameter("polynomial", ui->initseedInput);
    m_stateHelper->addLineEditStringParameter("taps", ui->tapInput);

    m_stateHelper->addParameter("bits_wanted", [this](QJsonValue value) {
        ui->bitsWantedInput->setText(QString("%1").arg(value.toInt()));
        return true;
    }, [this]() {
        MathParser mp;
        MathParser::ParseResult a = mp.parseInput(ui->bitsWantedInput->text());
        if (a.isValid()) {
            return QJsonValue(a.getResult());
        }
        else {
            return QJsonValue();
        }
    });

    connect(ui->btnTaps, SIGNAL(clicked()), this, SLOT(showHelp()));
}

LfsrForm::~LfsrForm()
{
    delete ui;
}

QString LfsrForm::title()
{
    return "Configure LFSR";
}

bool LfsrForm::setParameters(const Parameters &parameters)
{
    return m_stateHelper->applyParametersToUi(parameters);
}

Parameters LfsrForm::parameters()
{
    return m_stateHelper->getParametersFromUi();
}

void LfsrForm::updateEndState(QString lfsr)
{
    ui->endStateOutput->setText(lfsr);
}

/*
 * Displays message box indicating
 * how to properly input which taps you're using
 */
void LfsrForm::showHelp()
{
    QMessageBox msg;
    msg.setText("Important Note:");
    msg.setInformativeText("Separate each tap by a ','");
    msg.setDefaultButton(QMessageBox::Close);
    msg.exec();

}
