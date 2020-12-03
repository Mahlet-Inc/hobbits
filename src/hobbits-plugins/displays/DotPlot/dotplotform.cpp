#include "dotplotform.h"
#include "ui_dotplotform.h"

DotPlotForm::DotPlotForm(QSharedPointer<ParameterDelegate> delegate):
    ui(new Ui::DotPlotForm()),
    m_paramHelper(new ParameterHelper(delegate))
{
    ui->setupUi(this);

    connect(ui->sb_wordSize, SIGNAL(valueChanged(int)), this, SIGNAL(changed()));
    connect(ui->sb_windowSize, SIGNAL(valueChanged(int)), this, SIGNAL(changed()));
    connect(ui->hs_scale, SIGNAL(valueChanged(int)), this, SIGNAL(changed()));

    m_paramHelper->addSliderIntParameter("scale", ui->hs_scale);
    m_paramHelper->addSpinBoxIntParameter("window_size", ui->sb_windowSize);
    m_paramHelper->addSpinBoxIntParameter("word_size", ui->sb_wordSize);
}

DotPlotForm::~DotPlotForm()
{
    delete ui;
}

QString DotPlotForm::title()
{
    return "Configure Dot Plot";
}

QJsonObject DotPlotForm::parameters()
{
    return m_paramHelper->getParametersFromUi();
}

bool DotPlotForm::setParameters(QJsonObject parameters)
{
    return m_paramHelper->applyParametersToUi(parameters);
}
