#include "byterastercontrols.h"
#include "ui_byterastercontrols.h"


ByteRasterControls::ByteRasterControls() :
    ui(new Ui::ByteRasterControls())
{
    ui->setupUi(this);

    connect(ui->hs_scale, SIGNAL(valueChanged(int)), this, SIGNAL(scaleSet(int)));
}

void ByteRasterControls::on_cb_showHeaders_stateChanged(int state)
{
    emit showHeadersChanged(state != Qt::Unchecked);
}
