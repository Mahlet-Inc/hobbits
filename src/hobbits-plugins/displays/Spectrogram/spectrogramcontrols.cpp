#include "spectrogramcontrols.h"
#include "ui_spectrogramcontrols.h"
#include "spectrogramwidget.h"


SpectrogramControls::SpectrogramControls() :
    ui(new Ui::SpectrogramControls())
{
    ui->setupUi(this);

    ui->cb_wordFormat->addItem("Unsigned Integer", SpectrogramWidget::Unsigned);
    ui->cb_wordFormat->addItem("Two's Complement", SpectrogramWidget::TwosComplement);

    ui->cb_dataType->addItem("Real", SpectrogramWidget::Real);
    ui->cb_dataType->addItem("Real and Complex Interleaved", SpectrogramWidget::RealComplexInterleaved);

    connect(ui->sb_wordSize, SIGNAL(valueChanged(int)), this, SIGNAL(wordSizeSet(int)));
    connect(ui->sb_fftSize, SIGNAL(valueChanged(int)), this, SIGNAL(fftSizeSet(int)));
    connect(ui->sb_overlap, SIGNAL(valueChanged(int)), this, SIGNAL(overlapSet(int)));
}

void SpectrogramControls::sendCurrentValues()
{
    emit wordSizeSet(ui->sb_wordSize->value());
    emit overlapSet(ui->sb_overlap->value());
    emit fftSizeSet(ui->sb_fftSize->value());
    emit wordFormatSet(ui->cb_wordFormat->currentData().toInt());
    emit dataTypeSet(ui->cb_dataType->currentData().toInt());
}

void SpectrogramControls::on_cb_wordFormat_currentIndexChanged(int index)
{
    emit wordFormatSet(ui->cb_wordFormat->itemData(index).toInt());
}

void SpectrogramControls::on_cb_dataType_currentIndexChanged(int index)
{
    emit dataTypeSet(ui->cb_dataType->itemData(index).toInt());
}

void SpectrogramControls::on_hs_sensitivity_valueChanged(int value)
{
    double sensitivity;
    if (value < 2500) {
        double decrease = double(value - 2500);
        sensitivity = 1.0 - (decrease*decrease)/(2500*2500);
    }
    else {
        double increase = double(value - 2500);
        sensitivity = 1.0 + 256*(increase*increase)/(2500*2500);
    }
    emit sensitivitySet(sensitivity);
}
