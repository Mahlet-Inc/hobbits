#include "spectrogramcontrols.h"
#include "ui_spectrogramcontrols.h"
#include "spectrogramrenderer.h"
#include "metadatahelper.h"

SpectrogramControls::SpectrogramControls() :
    ui(new Ui::SpectrogramControls())
{
    ui->setupUi(this);

    ui->cb_rateUnits->addItem("Hz", 1.0);
    ui->cb_rateUnits->addItem("kHz", 1000.0);
    ui->cb_rateUnits->addItem("MHz", 1000000.0);
    ui->cb_rateUnits->addItem("GHz", 1000000000.0);
    ui->cb_rateUnits->addItem("THz", 1000000000000.0);

    for (auto format : MetadataHelper::sampleFormats()) {
        ui->cb_sampleFormat->addItem(format.name, format.id);
    }

    ui->cb_dataType->addItem("Real", SpectrogramRenderer::Real);
    ui->cb_dataType->addItem("Real and Complex Interleaved", SpectrogramRenderer::RealComplexInterleaved);

    connect(ui->sb_fftSize, SIGNAL(valueChanged(int)), this, SIGNAL(fftSizeSet(int)));
    connect(ui->sb_overlap, SIGNAL(valueChanged(int)), this, SIGNAL(overlapSet(int)));
    connect(ui->ck_showHeaders, SIGNAL(toggled(bool)), this, SIGNAL(headersShowSet(bool)));
    connect(ui->ck_hoverSlices, SIGNAL(toggled(bool)), this, SIGNAL(sliceShowSet(bool)));
    connect(ui->ck_logarithmic, SIGNAL(toggled(bool)), this, SIGNAL(logarithmicSet(bool)));
}

void SpectrogramControls::sendCurrentValues()
{
    emit overlapSet(ui->sb_overlap->value());
    emit fftSizeSet(ui->sb_fftSize->value());
    emit sampleFormatSet(ui->cb_sampleFormat->currentData().toString());
    emit dataTypeSet(ui->cb_dataType->currentData().toInt());
    emit sampleRateSet(ui->sb_sampleRate->value() * ui->cb_rateUnits->currentData().toDouble());
    emit headersShowSet(ui->ck_showHeaders->isChecked());
    emit sliceShowSet(ui->ck_hoverSlices->isChecked());
    emit logarithmicSet(ui->ck_logarithmic->isChecked());
}

void SpectrogramControls::setSampleFormat(QString id)
{
    int idx = ui->cb_sampleFormat->findData(id);
    if (idx >= 0) {
        ui->cb_sampleFormat->setCurrentIndex(idx);
    }
}

void SpectrogramControls::on_cb_sampleFormat_currentIndexChanged(int index)
{
    emit sampleFormatSet(ui->cb_sampleFormat->itemData(index).toString());
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
        sensitivity = 1.0 + decrease/2500;
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
