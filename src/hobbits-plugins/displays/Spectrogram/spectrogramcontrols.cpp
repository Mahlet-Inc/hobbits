#include "spectrogramcontrols.h"
#include "ui_spectrogramcontrols.h"
#include "spectrogramwidget.h"


SpectrogramControls::SpectrogramControls() :
    ui(new Ui::SpectrogramControls())
{
    ui->setupUi(this);

    ui->cb_rateUnits->addItem("Hz", 1.0);
    ui->cb_rateUnits->addItem("kHz", 1000.0);
    ui->cb_rateUnits->addItem("MHz", 1000000.0);
    ui->cb_rateUnits->addItem("GHz", 1000000000.0);
    ui->cb_rateUnits->addItem("THz", 1000000000000.0);

    ui->cb_wordFormat->addItem("Unsigned Integer", SpectrogramWidget::Unsigned);
    ui->cb_wordFormat->addItem("Two's Complement", SpectrogramWidget::TwosComplement);
    ui->cb_wordFormat->addItem("IEEE 754 Floating Point", SpectrogramWidget::IEEE_754);

    ui->cb_endianness->addItem("Big Endian", SpectrogramWidget::BigEndian);
    ui->cb_endianness->addItem("Little Endian", SpectrogramWidget::LittleEndian);

    ui->cb_dataType->addItem("Real", SpectrogramWidget::Real);
    ui->cb_dataType->addItem("Real and Complex Interleaved", SpectrogramWidget::RealComplexInterleaved);

    connect(ui->sb_wordSize, SIGNAL(valueChanged(int)), this, SIGNAL(wordSizeSet(int)));
    connect(ui->sb_fftSize, SIGNAL(valueChanged(int)), this, SIGNAL(fftSizeSet(int)));
    connect(ui->sb_overlap, SIGNAL(valueChanged(int)), this, SIGNAL(overlapSet(int)));
    connect(ui->ck_showHeaders, SIGNAL(toggled(bool)), this, SIGNAL(headersShowSet(bool)));
}

void SpectrogramControls::sendCurrentValues()
{
    emit wordSizeSet(ui->sb_wordSize->value());
    emit overlapSet(ui->sb_overlap->value());
    emit fftSizeSet(ui->sb_fftSize->value());
    emit wordFormatSet(ui->cb_wordFormat->currentData().toInt() | ui->cb_endianness->currentData().toInt());
    emit dataTypeSet(ui->cb_dataType->currentData().toInt());
    emit sampleRateSet(ui->sb_sampleRate->value() * ui->cb_rateUnits->currentData().toDouble());
    emit headersShowSet(ui->ck_showHeaders->isChecked());
}

void SpectrogramControls::on_cb_wordFormat_currentIndexChanged(int index)
{
    emit wordFormatSet(ui->cb_wordFormat->itemData(index).toInt() | ui->cb_endianness->currentData().toInt());
}

void SpectrogramControls::on_cb_endianness_currentIndexChanged(int index)
{
    emit wordFormatSet(ui->cb_wordFormat->currentData().toInt() | ui->cb_endianness->itemData(index).toInt());
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


void SpectrogramControls::on_sb_sampleRate_valueChanged(double arg1)
{
    emit sampleRateSet(ui->sb_sampleRate->value() * ui->cb_rateUnits->currentData().toDouble());
}

void SpectrogramControls::on_cb_rateUnits_currentIndexChanged(int index)
{
    emit sampleRateSet(ui->sb_sampleRate->value() * ui->cb_rateUnits->currentData().toDouble());
}
