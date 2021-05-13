#include "bitrastercontrols.h"
#include "ui_bitrastercontrols.h"


BitRasterControls::BitRasterControls(QSharedPointer<ParameterDelegate> delegate) :
    ui(new Ui::BitRasterControls()),
    m_stateHelper(new ParameterHelper(delegate))
{
    ui->setupUi(this);

    connect(ui->hs_scale, SIGNAL(valueChanged(int)), this, SIGNAL(changed()));
    connect(ui->cb_showHeaders, SIGNAL(stateChanged(int)), this, SIGNAL(changed()));

    m_stateHelper->addSliderIntParameter("scale", ui->hs_scale);
    m_stateHelper->addCheckBoxBoolParameter("show_headers", ui->cb_showHeaders);
}

QString BitRasterControls::title()
{
    return "Configure Bit Raster";
}

bool BitRasterControls::setParameters(const Parameters &parameters)
{
    return m_stateHelper->applyParametersToUi(parameters);
}

Parameters BitRasterControls::parameters()
{
    return m_stateHelper->getParametersFromUi();
}
