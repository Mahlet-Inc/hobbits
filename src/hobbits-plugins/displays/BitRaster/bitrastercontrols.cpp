#include "bitrastercontrols.h"
#include "ui_bitrastercontrols.h"


BitRasterControls::BitRasterControls() :
    ui(new Ui::BitRasterControls())
{
    ui->setupUi(this);

    connect(ui->hs_scale, SIGNAL(valueChanged(int)), this, SIGNAL(scaleSet(int)));
}

void BitRasterControls::on_cb_showHeaders_stateChanged(int state)
{
    emit showHeadersChanged(state != Qt::Unchecked);
}
