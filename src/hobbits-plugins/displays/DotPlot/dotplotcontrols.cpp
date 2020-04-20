#include "dotplotcontrols.h"
#include "ui_dotplotcontrols.h"


DotPlotControls::DotPlotControls() :
    ui(new Ui::DotPlotControls())
{
    ui->setupUi(this);

    connect(ui->sb_wordSize, SIGNAL(valueChanged(int)), this, SIGNAL(newWordSize(int)));
    connect(ui->sb_windowSize, SIGNAL(valueChanged(int)), this, SIGNAL(newWindowSize(int)));
    connect(ui->sb_scale, SIGNAL(valueChanged(int)), this, SIGNAL(newScale(int)));
}
