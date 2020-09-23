#ifndef WIDTHFRAMER_H
#define WIDTHFRAMER_H

#include "analyzerinterface.h"
#include "peakselector.h"

#ifdef FFTW_AUTOCORRELATION
#include <fftw3.h>
#endif

namespace Ui
{
class WidthFramer;

}

class WidthFramer : public QObject, AnalyzerInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "hobbits.AnalyzerInterface.WidthFramer")
    Q_INTERFACES(AnalyzerInterface)

public:
    WidthFramer();

    ~WidthFramer() override;

    QString name() override;
    void previewBits(QSharedPointer<BitContainerPreview> container) override;
    void provideCallback(QSharedPointer<PluginCallback> pluginCallback) override;

    AnalyzerInterface* createDefaultAnalyzer() override;
    void applyToWidget(QWidget *widget) override;

    bool canRecallPluginState(const QJsonObject &pluginState) override;
    virtual bool setPluginStateInUi(const QJsonObject &pluginState) override;
    QJsonObject getStateFromUi() override;
    QSharedPointer<const AnalyzerResult> analyzeBits(
            QSharedPointer<const BitContainer> container,
            const QJsonObject &recallablePluginState,
            QSharedPointer<ActionProgress> progressTracker) override;

    int getAutoWidth(QSharedPointer<BitArray> bits, int start, int end);
    QVector<QPointF> autocorrelate(QSharedPointer<const BitArray> bits);

public slots:
    void getPeak(QPointF);

    void setupScoreList(bool toggled = true);

private slots:
    void requestRun();
    void widthSelected(QModelIndex index);

private:
    Ui::WidthFramer *ui;
    QSharedPointer<PluginCallback> m_pluginCallback;

    BitArray getFrameAlignBits();

    PeakSelector *m_peakSelector;

    QVector<QPointF> m_autocorrelation;

    QStringListModel *m_listModel;

#ifdef FFTW_AUTOCORRELATION
    fftw_complex *m_fft_in;
    fftw_complex *m_fft_out;
    int m_fftSize;
    fftw_plan m_fft_plan1;
    fftw_plan m_fft_plan2;
#endif

};

#endif // WIDTHFRAMER_H
