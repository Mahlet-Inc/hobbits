#include "asciiform.h"
#include "ui_asciiform.h"

AsciiForm::AsciiForm(QSharedPointer<ParameterDelegate> delegate):
    ui(new Ui::AsciiForm()),
    m_paramHelper(new ParameterHelper(delegate))
{
    ui->setupUi(this);

    ui->cb_encoding->addItem("ASCII", "ASCII");
    ui->cb_encoding->addItem("CP437", "CP437");

    connect(ui->sb_columnGrouping, SIGNAL(valueChanged(int)), this, SIGNAL(changed()));
    connect(ui->hs_fontSize, SIGNAL(valueChanged(int)), this, SIGNAL(changed()));
    connect(ui->cb_showHeaders, SIGNAL(stateChanged(int)), this, SIGNAL(changed()));
    connect(ui->cb_encoding, SIGNAL(currentIndexChanged(int)), this, SIGNAL(changed()));

    m_paramHelper->addSliderIntParameter("font_size", ui->hs_fontSize);
    m_paramHelper->addCheckBoxBoolParameter("show_headers", ui->cb_showHeaders);
    m_paramHelper->addSpinBoxIntParameter("column_grouping", ui->sb_columnGrouping);
    m_paramHelper->addComboBoxParameter("encoding", ui->cb_encoding);
}

AsciiForm::~AsciiForm()
{
    delete ui;
}

QString AsciiForm::title()
{
    return "Configure Ascii Display";
}

Parameters AsciiForm::parameters()
{
    return m_paramHelper->getParametersFromUi();
}

bool AsciiForm::setParameters(const Parameters &parameters)
{
    return m_paramHelper->applyParametersToUi(parameters);
}
