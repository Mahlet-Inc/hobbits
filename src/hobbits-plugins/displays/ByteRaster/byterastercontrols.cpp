#include "byterastercontrols.h"
#include "ui_byterastercontrols.h"


ByteRasterControls::ByteRasterControls(QSharedPointer<ParameterDelegate> delegate) :
    ui(new Ui::ByteRasterControls()),
    m_stateHelper(new ParameterHelper(delegate))
{
    ui->setupUi(this);

    connect(ui->hs_scale, SIGNAL(valueChanged(int)), this, SIGNAL(changed()));
    connect(ui->cb_showHeaders, SIGNAL(stateChanged(int)), this, SIGNAL(changed()));

    m_stateHelper->addSliderIntParameter("scale", ui->hs_scale);
    m_stateHelper->addCheckBoxBoolParameter("show_headers", ui->cb_showHeaders);
}

QString ByteRasterControls::title()
{
    return "Configure Byte Raster";
}

bool ByteRasterControls::setParameters(const Parameters &parameters)
{
    return m_stateHelper->applyParametersToUi(parameters);
}

Parameters ByteRasterControls::parameters()
{
    return m_stateHelper->getParametersFromUi();
}

