#include "spectrogramwidget.h"

#include <QPainter>
#include <QImage>
#include "displayhelper.h"
#include "fftw3.h"
#include <QtMath>
#include "settingsmanager.h"

#include <QDebug>

SpectrogramWidget::SpectrogramWidget(
        QSharedPointer<DisplayHandle> displayHandle,
        DisplayInterface *pluginRef,
        QWidget *parent) :
    DisplayBase(displayHandle, pluginRef, parent),
    m_scale(1),
    m_showFrameOffsets(false),
    m_showColumnOffsets(false),
    m_wordSize(8),
    m_stride(512),
    m_fftSizeFactor(4),
    m_displayOffset(0, 0),
    m_headerFontSize(0, 0)
{

}

void SpectrogramWidget::paintEvent(QPaintEvent*) {
    if (m_displayHandle->getContainer().isNull()) {
        return;
    }

    int frameOffset = m_displayHandle->getFrameOffset();
    if (frameOffset < 0 || frameOffset >= m_displayHandle->getContainer()->frames().size()) {
        return;
    }

    int displayWidth = (this->width() - m_displayOffset.x()) / m_scale;
    int displayHeight = (this->height() - m_displayOffset.y()) / m_scale;

    int fftSize = m_stride*m_fftSizeFactor;

    qint64 bitOffset = m_displayHandle->getContainer()->frames().at(frameOffset).start();
    auto spectrums = computeStft(m_wordSize, fftSize, m_stride, displayHeight, bitOffset, m_displayHandle->getContainer());

    QColor c = SettingsManager::getInstance().getUiSetting(SettingsData::BYTE_HUE_SAT_KEY).value<QColor>();
    qreal hue = c.hueF();
    qreal saturation = c.saturationF();
    QImage raster(fftSize/2, displayHeight, QImage::Format_ARGB32);
    raster.fill(qRgba(0x00, 0x00, 0x00, 0xff));
    for (int y = 0; y < spectrums.size(); y++) {
        auto spectrum = spectrums.at(y);
        for (int x = 0; x < fftSize/2  && x < spectrum.size(); x++) {
            qreal lightness = spectrum.at(x);
            c.setHslF(hue, saturation, lightness);
            raster.setPixel(x, y, c.rgba());
        }
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.drawImage(QRect(m_displayOffset.x(), m_displayOffset.y(), displayWidth, displayHeight), raster);
}

void SpectrogramWidget::mouseMoveEvent(QMouseEvent *event) {
    sendHoverUpdate(event, 1, 1, 1, 1, QPoint(0, 0));
}

void SpectrogramWidget::setScale(int val)
{
    m_scale = val;
    adjustScrollbars();
}

void SpectrogramWidget::setStride(int val)
{
    m_stride = val;
    repaint();
}

void SpectrogramWidget::setFftSizeFactor(int val)
{
    m_fftSizeFactor = val;
    repaint();
}

void SpectrogramWidget::setWordSize(int val)
{
    m_wordSize = val;
    repaint();
}

QList<QVector<double>> SpectrogramWidget::computeStft(int wordSize, int windowSize, int stride, int maxSpectrums, qint64 bitOffset, QSharedPointer<BitContainer> container)
{
    QList<QVector<double>> spectrums;
    if (wordSize > 64 || wordSize < 1) {
        return spectrums;
    }

    int sampleCount = maxSpectrums*stride + windowSize;
    double *samples = new double[sampleCount];
    int actualSamples = 0;
    quint64 maxWordValue = 1;
    maxWordValue <<= (wordSize - 1);
    double wordInverse = 1.0 / double(maxWordValue);
    for (int i = 0; i < sampleCount; i++) {
        qint64 offset = bitOffset + i*wordSize;
        if (offset+wordSize >= container->bits()->sizeInBits()) {
            break;
        }
        samples[i] = double(container->bits()->getWordValueTwosComplement(offset, wordSize)) * wordInverse;
        actualSamples++;
    }

    fftw_complex *fft_out = reinterpret_cast<fftw_complex*>(fftw_malloc(sizeof(fftw_complex) * unsigned(windowSize)));
    double *currSamples = samples;
    fftw_plan plan = fftw_plan_dft_r2c_1d(windowSize, currSamples, fft_out, FFTW_ESTIMATE);

    double outputFactor = 2.0 / double(windowSize);

    for (int i = 0; i < maxSpectrums; i++) {
        if ((currSamples-samples) + windowSize >= actualSamples) {
            break;
        }
        fftw_execute_dft_r2c(plan, currSamples, fft_out);

        QVector<double> spectrum(windowSize/2);
        for (int n = 0; n < windowSize/2; n++) {
            spectrum[n] = qMin(1.0, (fft_out[n][0] * fft_out[n][0] * outputFactor) + (fft_out[n][1] * fft_out[n][1] * outputFactor));
        }
        spectrums.append(spectrum);
        currSamples += stride;
    }

    fftw_destroy_plan(plan);
    fftw_free(fft_out);
    delete[] samples;

    return spectrums;
}

void SpectrogramWidget::prepareHeaders()
{
    if (m_displayHandle->getContainer().isNull()) {
        return;
    }

    QPainter painter(this);
    QFont font = QFont("monospace", 10);
    font.setStyleStrategy(QFont::ForceIntegerMetrics);
    QFontMetrics fontMetrics = QFontMetrics(font, painter.device());
    int fontWidth = fontMetrics.width("0");
    int fontHeight = fontMetrics.height();
    m_headerFontSize.setWidth(fontWidth);
    m_headerFontSize.setHeight(fontHeight);

    if (m_showFrameOffsets) {
        int totalFrames = m_displayHandle->getContainer()->frames().size();
        int maxChars = qFloor(log10(totalFrames)) + 1;
        m_displayOffset.setX(qRound(fontWidth * (maxChars + 1.5)));
    }
    else {
        m_displayOffset.setX(0);
    }

    if (m_showColumnOffsets) {
        int maxWidth = m_displayHandle->getContainer()->maxFrameWidth();
        int maxChars = qFloor(log10(maxWidth)) + 1;
        m_displayOffset.setY(fontWidth * (maxChars + 1));
    }
    else {
        m_displayOffset.setY(0);
    }
}

void SpectrogramWidget::adjustScrollbars()
{
    if (!m_displayHandle->getCurrentFocusDisplays().contains(m_pluginRef)) {
        return;
    }
    DisplayBase::adjustScrollbars();

    repaint();

    if (m_displayHandle->getCurrentFocusDisplays().size() == 1) {
        int h = (height() - m_displayOffset.y()) / m_scale;
        m_displayHandle->getVScroll()->setPageStep(h);
    }

    m_displayHandle->getHScroll()->setVisible(false);
}


