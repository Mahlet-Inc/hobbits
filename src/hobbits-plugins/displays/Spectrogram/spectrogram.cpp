#include "spectrogram.h"

Spectrogram::Spectrogram() :
    m_displayWidget(nullptr),
    m_controlsWidget(nullptr)
{

}

DisplayInterface* Spectrogram::createDefaultDisplay()
{
    return new Spectrogram();
}

QString Spectrogram::getName()
{
    return "Spectrogram";
}

QWidget* Spectrogram::getDisplayWidget(QSharedPointer<DisplayHandle> displayHandle)
{
    initialize(displayHandle);
    return m_displayWidget;
}

QWidget* Spectrogram::getControlsWidget(QSharedPointer<DisplayHandle> displayHandle)
{
    initialize(displayHandle);
    return m_controlsWidget;
}

void Spectrogram::initialize(QSharedPointer<DisplayHandle> displayHandle)
{
    if (!m_displayWidget) {
        m_displayWidget = new SpectrogramWidget(displayHandle, this);
        m_controlsWidget = new SpectrogramControls();

        connect(m_controlsWidget, &SpectrogramControls::overlapSet,
                m_displayWidget, &SpectrogramWidget::setOverlap);
        connect(m_controlsWidget, &SpectrogramControls::fftSizeSet,
                m_displayWidget, &SpectrogramWidget::setFftSize);
        connect(m_controlsWidget, &SpectrogramControls::sampleFormatSet,
                m_displayWidget, &SpectrogramWidget::setSampleFormat);
        connect(m_controlsWidget, &SpectrogramControls::dataTypeSet,
                m_displayWidget, &SpectrogramWidget::setDataType);
        connect(m_controlsWidget, &SpectrogramControls::sensitivitySet,
                m_displayWidget, &SpectrogramWidget::setSensitivity);
        connect(m_controlsWidget, &SpectrogramControls::sampleRateSet,
                m_displayWidget, &SpectrogramWidget::setSampleRate);
        connect(m_controlsWidget, &SpectrogramControls::headersShowSet,
                m_displayWidget, &SpectrogramWidget::setShowHeaders);
        connect(m_controlsWidget, &SpectrogramControls::sliceShowSet,
                m_displayWidget, &SpectrogramWidget::setShowHoverSlices);
        connect(m_controlsWidget, &SpectrogramControls::logarithmicSet,
                m_displayWidget, &SpectrogramWidget::setLogarithmic);
        connect(m_displayWidget, &SpectrogramWidget::sampleFormatChanged,
                m_controlsWidget, &SpectrogramControls::setSampleFormat);

        m_controlsWidget->sendCurrentValues();
    }
}
