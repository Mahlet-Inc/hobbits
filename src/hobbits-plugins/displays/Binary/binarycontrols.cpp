#include "binarycontrols.h"
#include "ui_binarycontrols.h"


BinaryControls::BinaryControls(QSharedPointer<ParameterDelegate> delegate) :
    ui(new Ui::BinaryControls()),
    m_stateHelper(new ParameterHelper(delegate))
{
    ui->setupUi(this);

    connect(ui->sb_columnGrouping, SIGNAL(valueChanged(int)), this, SIGNAL(changed()));
    connect(ui->hs_fontSize, SIGNAL(valueChanged(int)), this, SIGNAL(changed()));
    connect(ui->cb_showHeaders, SIGNAL(stateChanged(int)), this, SIGNAL(changed()));

    m_stateHelper->addSliderIntParameter("font_size", ui->hs_fontSize);
    m_stateHelper->addCheckBoxBoolParameter("show_headers", ui->cb_showHeaders);
    m_stateHelper->addSpinBoxIntParameter("column_grouping", ui->sb_columnGrouping);
}

QString BinaryControls::title()
{
    return "Configure Binary Display";
}

bool BinaryControls::setParameters(const Parameters &parameters)
{
    return m_stateHelper->applyParametersToUi(parameters);
}

Parameters BinaryControls::parameters()
{
    return m_stateHelper->getParametersFromUi();
}
