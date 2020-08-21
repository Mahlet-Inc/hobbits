#include "spectrogramcontrols.h"
#include "ui_spectrogramcontrols.h"


SpectrogramControls::SpectrogramControls() :
    ui(new Ui::SpectrogramControls())
{
    ui->setupUi(this);
    calculateFftSize();

    connect(ui->sb_wordSize, SIGNAL(valueChanged(int)), this, SIGNAL(wordSizeSet(int)));
}

void SpectrogramControls::sendCurrentValues()
{
    emit wordSizeSet(ui->sb_wordSize->value());
    emit strideSet(ui->sb_stride->value());
    emit fftFactorSet(ui->sb_fftFactor->value());
}

void SpectrogramControls::on_sb_stride_valueChanged(int val)
{
    calculateFftSize();
    emit strideSet(val);
}

void SpectrogramControls::on_sb_fftFactor_valueChanged(int val)
{
    calculateFftSize();
    emit fftFactorSet(val);
}

void SpectrogramControls::calculateFftSize()
{
    int fftSize = ui->sb_stride->value() * ui->sb_fftFactor->value();
    ui->lb_fftSize->setText(QString("FFT Size: %1").arg(fftSize));
}
