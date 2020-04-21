#include "frequencyplotcontrols.h"
#include "ui_frequencyplotcontrols.h"


FrequencyPlotControls::FrequencyPlotControls() :
    ui(new Ui::FrequencyPlotControls())
{
    ui->setupUi(this);

    connect(ui->sb_wordSize, SIGNAL(valueChanged(int)), this, SIGNAL(newWordSize(int)));
    connect(ui->sb_windowSize, SIGNAL(valueChanged(int)), this, SIGNAL(newWindowSize(int)));
    connect(ui->sb_scale, SIGNAL(valueChanged(int)), this, SIGNAL(newScale(int)));
}
