#include "frequencyplotform.h"
#include "ui_frequencyplotform.h"

FrequencyPlotForm::FrequencyPlotForm(QSharedPointer<ParameterDelegate> delegate):
    ui(new Ui::FrequencyPlotForm()),
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

FrequencyPlotForm::~FrequencyPlotForm()
{
    delete ui;
}

QString FrequencyPlotForm::title()
{
    return "Configure Frequency Plot";
}

Parameters FrequencyPlotForm::parameters()
{
    return m_paramHelper->getParametersFromUi();
}

bool FrequencyPlotForm::setParameters(const Parameters &parameters)
{
    return m_paramHelper->applyParametersToUi(parameters);
}
