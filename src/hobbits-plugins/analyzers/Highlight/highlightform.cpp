#include "highlightform.h"
#include "ui_highlightform.h"

HighlightForm::HighlightForm(QSharedPointer<ParameterDelegate> delegate):
    ui(new Ui::HighlightForm()),
    m_helper(new ParameterHelper(delegate))
{
    ui->setupUi(this);

    m_helper->addSpinBoxIntParameter("start", ui->sb_start);
    m_helper->addSpinBoxIntParameter("length", ui->sb_length);
}

HighlightForm::~HighlightForm()
{
    delete ui;
}

QString HighlightForm::title()
{
    return "Configure Plugin";
}

QJsonObject HighlightForm::parameters()
{
    return m_helper->getParametersFromUi();
}

bool HighlightForm::setParameters(QJsonObject parameters)
{
    return m_helper->applyParametersToUi(parameters);
}
