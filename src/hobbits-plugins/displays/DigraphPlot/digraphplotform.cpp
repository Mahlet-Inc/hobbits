#include "digraphplotform.h"
#include "ui_digraphplotform.h"

DigraphPlotForm::DigraphPlotForm(QSharedPointer<ParameterDelegate> delegate):
    ui(new Ui::DigraphPlotForm()),
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

DigraphPlotForm::~DigraphPlotForm()
{
    delete ui;
}

QString DigraphPlotForm::title()
{
    return "Configure Digraph Plot";
}

QJsonObject DigraphPlotForm::parameters()
{
    return m_paramHelper->getParametersFromUi();
}

bool DigraphPlotForm::setParameters(QJsonObject parameters)
{
    return m_paramHelper->applyParametersToUi(parameters);
}
