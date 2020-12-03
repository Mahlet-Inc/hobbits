#include "spectrogramcontrols.h"
#include "ui_spectrogramcontrols.h"
#include "metadatahelper.h"

SpectrogramControls::SpectrogramControls(QSharedPointer<ParameterDelegate> delegate) :
    ui(new Ui::SpectrogramControls()),
    m_paramHelper(new ParameterHelper(delegate))
{
    ui->setupUi(this);

    ui->cb_rateUnits->addItem("Hz", 1.0);
    ui->cb_rateUnits->addItem("kHz", 1000.0);
    ui->cb_rateUnits->addItem("MHz", 1000000.0);
    ui->cb_rateUnits->addItem("GHz", 1000000000.0);
    ui->cb_rateUnits->addItem("THz", 1000000000000.0);
    m_paramHelper->addParameter("sample_rate", [this](QJsonValue value) {
        double rate = value.toDouble();
        ui->sb_sampleRate->setValue(rate / 1000.0);
        ui->cb_rateUnits->setCurrentText("kHz");
        return true;
    }, [this]() {
        return ui->sb_sampleRate->value() * ui->cb_rateUnits->currentData().toDouble();
    });

    for (auto format : MetadataHelper::sampleFormats()) {
        ui->cb_sampleFormat->addItem(format.name, format.id);
    }
    m_paramHelper->addComboBoxParameter("sample_format", ui->cb_sampleFormat);

    ui->cb_dataType->addItem("Real", SpectrogramControls::Real);
    ui->cb_dataType->addItem("Real and Complex Interleaved", SpectrogramControls::RealComplexInterleaved);
    m_paramHelper->addComboBoxParameter("data_type", ui->cb_dataType);

    m_paramHelper->addSliderIntParameter("sensitivity", ui->hs_sensitivity);
    m_paramHelper->addSpinBoxIntParameter("fft_size", ui->sb_fftSize);
    m_paramHelper->addSpinBoxIntParameter("fft_overlap", ui->sb_overlap);

    m_paramHelper->addCheckBoxBoolParameter("show_headers", ui->ck_showHeaders);
    m_paramHelper->addCheckBoxBoolParameter("show_slices", ui->ck_hoverSlices);
    m_paramHelper->addCheckBoxBoolParameter("logarithmic_scaling", ui->ck_logarithmic);

    connect(ui->hs_sensitivity, SIGNAL(valueChanged(int)), this, SIGNAL(changed()));
    connect(ui->sb_fftSize, SIGNAL(valueChanged(int)), this, SIGNAL(changed()));
    connect(ui->sb_overlap, SIGNAL(valueChanged(int)), this, SIGNAL(changed()));
    connect(ui->ck_showHeaders, SIGNAL(toggled(bool)), this, SIGNAL(changed()));
    connect(ui->ck_hoverSlices, SIGNAL(toggled(bool)), this, SIGNAL(changed()));
    connect(ui->ck_logarithmic, SIGNAL(toggled(bool)), this, SIGNAL(changed()));

    connect(ui->cb_sampleFormat, SIGNAL(currentIndexChanged(int)), this, SIGNAL(changed()));
    connect(ui->cb_dataType, SIGNAL(currentIndexChanged(int)), this, SIGNAL(changed()));

    connect(ui->sb_sampleRate, SIGNAL(valueChanged(double)), this, SIGNAL(changed()));
    connect(ui->cb_rateUnits, SIGNAL(currentIndexChanged(int)), this, SIGNAL(changed()));
}

QString SpectrogramControls::title()
{
    return "Configure Spectrogram";
}

bool SpectrogramControls::setParameters(QJsonObject parameters)
{
    return m_paramHelper->applyParametersToUi(parameters);
}

QJsonObject SpectrogramControls::parameters()
{
    return m_paramHelper->getParametersFromUi();
}

void SpectrogramControls::previewBitsUiImpl(QSharedPointer<BitContainerPreview> container)
{
    if (container.isNull()) {
        return;
    }

    QString currId = ui->cb_sampleFormat->currentData().toString();
    QString name = container->name();

    QVariant sampleFormat = container->info()->metadata(MetadataHelper::sampleFormatKey());
    if (sampleFormat.isValid()) {
        int idx = ui->cb_sampleFormat->findData(sampleFormat.toString());
        if (idx >= 0) {
            ui->cb_sampleFormat->setCurrentIndex(idx);
        }
        else {
            container->setMetadata(MetadataHelper::sampleFormatKey(), currId);
        }
    }
    else {
        container->setMetadata(MetadataHelper::sampleFormatKey(), currId);
    }
}
