#ifndef SPECTROGRAM_H
#define SPECTROGRAM_H

#include "displayinterface.h"
#include "spectrogramcontrols.h"
#include "fftw3.h"
#include "metadatahelper.h"

class Spectrogram : public QObject, DisplayInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "hobbits.DisplayInterface.Spectrogram")
    Q_INTERFACES(DisplayInterface)

public:
    Spectrogram();

    DisplayInterface* createDefaultDisplay() override;

    QString name() override;
    QString description() override;
    QStringList tags() override;

    QSharedPointer<DisplayRenderConfig> renderConfig() override;
    void setDisplayHandle(QSharedPointer<DisplayHandle> displayHandle) override;
    QSharedPointer<ParameterDelegate> parameterDelegate() override;

    QSharedPointer<DisplayResult> renderDisplay(
            QSize viewportSize,
            const Parameters &parameters,
            QSharedPointer<PluginActionProgress> progress) override;

    QSharedPointer<DisplayResult> renderOverlay(
            QSize viewportSize,
            const Parameters &parameters) override;

private:
    QRect spectrogramRectangle(QSize viewportSize,
                           QSharedPointer<DisplayHandle> displayHandle,
                           const Parameters &parameters);
    QString timeString(qint64 sample, double sampleRate);
    void fillSamples(fftw_complex* buffer,
                        qint64 offset,
                        QSharedPointer<BitContainer> bitContainer,
                        const MetadataHelper::SampleFormat &sampleFormat,
                        int fftSize,
                        SpectrogramControls::DataType dataType);

    void setSpectrums(QList<QVector<double>> spectrums);

    QSharedPointer<ParameterDelegate> m_delegate;
    QSharedPointer<DisplayRenderConfig> m_renderConfig;
    QSharedPointer<DisplayHandle> m_handle;

    QMutex m_mutex;
    QList<QVector<double>> m_spectrums;
};

#endif // SPECTROGRAM_H
