#include "binaryviewcontrols.h"
#include "ui_binaryviewcontrols.h"


BinaryViewControls::BinaryViewControls() :
    ui(new Ui::BinaryViewControls())
{
    ui->setupUi(this);

    connect(ui->sb_columnGrouping, SIGNAL(valueChanged(int)), this, SIGNAL(columnGroupingChanged(int)));
    connect(ui->hs_fontSize, SIGNAL(valueChanged(int)), this, SIGNAL(fontSizeChanged(int)));
}

void BinaryViewControls::on_cb_showHeaders_stateChanged(int state)
{
    emit showHeadersChanged(state != Qt::Unchecked);
}
