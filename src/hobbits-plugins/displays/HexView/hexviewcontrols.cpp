#include "hexviewcontrols.h"
#include "ui_hexviewcontrols.h"


HexViewControls::HexViewControls() :
    ui(new Ui::HexViewControls())
{
    ui->setupUi(this);

    connect(ui->sb_columnGrouping, SIGNAL(valueChanged(int)), this, SIGNAL(columnGroupingChanged(int)));
    connect(ui->hs_fontSize, SIGNAL(valueChanged(int)), this, SIGNAL(fontSizeChanged(int)));
}

void HexViewControls::on_cb_showHeaders_stateChanged(int state)
{
    emit showHeadersChanged(state != Qt::Unchecked);
}
