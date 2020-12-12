#include "hexform.h"
#include "ui_hexform.h"

HexForm::HexForm(QSharedPointer<ParameterDelegate> delegate):
    ui(new Ui::HexForm()),
    m_paramHelper(new ParameterHelper(delegate))
{
    ui->setupUi(this);

    connect(ui->sb_columnGrouping, SIGNAL(valueChanged(int)), this, SIGNAL(changed()));
    connect(ui->hs_fontSize, SIGNAL(valueChanged(int)), this, SIGNAL(changed()));
    connect(ui->cb_showHeaders, SIGNAL(stateChanged(int)), this, SIGNAL(changed()));

    m_paramHelper->addSliderIntParameter("font_size", ui->hs_fontSize);
    m_paramHelper->addCheckBoxBoolParameter("show_headers", ui->cb_showHeaders);
    m_paramHelper->addSpinBoxIntParameter("column_grouping", ui->sb_columnGrouping);
}

HexForm::~HexForm()
{
    delete ui;
}

QString HexForm::title()
{
    return "Configure Hex Display";
}

QJsonObject HexForm::parameters()
{
    return m_paramHelper->getParametersFromUi();
}

bool HexForm::setParameters(QJsonObject parameters)
{
    return m_paramHelper->applyParametersToUi(parameters);
}
