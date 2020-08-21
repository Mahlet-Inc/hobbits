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

        connect(m_controlsWidget, &SpectrogramControls::wordSizeSet,
                m_displayWidget, &SpectrogramWidget::setWordSize);
        connect(m_controlsWidget, &SpectrogramControls::strideSet,
                m_displayWidget, &SpectrogramWidget::setStride);
        connect(m_controlsWidget, &SpectrogramControls::fftFactorSet,
                m_displayWidget, &SpectrogramWidget::setFftSizeFactor);

        m_controlsWidget->sendCurrentValues();
    }
}
