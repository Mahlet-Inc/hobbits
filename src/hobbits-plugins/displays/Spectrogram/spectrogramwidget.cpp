#include "spectrogramwidget.h"

#include <QPainter>
#include <QImage>
#include "displayhelper.h"
#include "fftw3.h"
#include <QtMath>
#include <QMouseEvent>
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
    m_overlap(4),
    m_fftSize(2048),
    m_wordFormat(TwosComplement),
    m_dataType(Real),
    m_sensitivity(1.0),
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

    qint64 bitOffset = m_displayHandle->getContainer()->frames().at(frameOffset).start();
    auto spectrums = computeStft(displayHeight, bitOffset, m_displayHandle->getContainer());

    QColor c = SettingsManager::getInstance().getUiSetting(SettingsData::BYTE_HUE_SAT_KEY).value<QColor>();
    qreal hue = c.hueF();
    qreal saturation = c.saturationF();
    QImage raster(m_fftSize/2, displayHeight, QImage::Format_ARGB32);
    raster.fill(qRgba(0x00, 0x00, 0x00, 0xff));
    for (int y = 0; y < spectrums.size(); y++) {
        auto spectrum = spectrums.at(y);
        for (int x = 0; x < m_fftSize/2  && x < spectrum.size(); x++) {
            qreal lightness = qBound(0.0, spectrum.at(x)*m_sensitivity, 1.0);
            c.setHslF(hue, saturation, lightness);
            raster.setPixel(x, y, c.rgba());
        }
    }

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.drawImage(QRect(m_displayOffset.x(), m_displayOffset.y(), displayWidth*m_scale, displayHeight*m_scale), raster);
}

void SpectrogramWidget::mouseMoveEvent(QMouseEvent *event) {
    if (m_displayHandle->getContainer().isNull()) {
        emit bitHover(false, 0, 0);
        return;
    }


    int y = (event->y() - m_displayOffset.y())/m_scale;
    if (y < 0) {
        emit bitHover(false, 0, 0);
        return;
    }
    int baseFrameOffset = m_displayHandle->getFrameOffset();
    if (baseFrameOffset < 0 || baseFrameOffset > m_displayHandle->getContainer()->frames().size()) {
        emit bitHover(false, 0, 0);
        return;
    }
    qint64 baseBitOffset = m_displayHandle->getContainer()->frames().at(baseFrameOffset).start();
    int bitsPerY = bitStride();
    qint64 bitOffset = y * bitsPerY + baseBitOffset;
    if (bitOffset >= m_displayHandle->getContainer()->bits()->sizeInBits()) {
        emit bitHover(false, 0, 0);
        return;
    }
    int frameOffset = m_displayHandle->getContainer()->bitInfo()->frameOffsetContaining(Range(bitOffset, bitOffset));

    emit bitHover(event, 0, frameOffset);
}

void SpectrogramWidget::setScale(int val)
{
    m_scale = val;
    adjustScrollbars();
}

void SpectrogramWidget::setOverlap(int val)
{
    m_overlap = val;
    repaint();
}

void SpectrogramWidget::setFftSize(int val)
{
    m_fftSize = val;
    repaint();
}

void SpectrogramWidget::setWordSize(int val)
{
    m_wordSize = val;
    repaint();
}

void SpectrogramWidget::setWordFormat(int val)
{
    m_wordFormat = static_cast<WordFormat>(val);
    repaint();
}

void SpectrogramWidget::setDataType(int val)
{
    m_dataType = static_cast<DataType>(val);
    repaint();
}

void SpectrogramWidget::setSensitivity(double val)
{
    m_sensitivity = val;
    repaint();
}

int SpectrogramWidget::bitStride()
{
    int strideBits = m_fftSize;
    if (m_overlap > 0) {
        strideBits = m_fftSize / m_overlap;
    }
    strideBits *= m_wordSize;
    return strideBits;
}

void SpectrogramWidget::fillSamples(fftw_complex* buffer, int sampleCount, qint64 bitOffset, QSharedPointer<BitContainer> container)
{
    quint64 maxWordValue = 1;
    maxWordValue <<= (m_wordSize - 1);
    double wordInverse = 1.0 / double(maxWordValue);
    int sampleSize = m_wordSize;
    bool withComplex = (m_dataType == RealComplexInterleaved);
    if (withComplex) {
        sampleSize *= 2;
    }
    for (int i = 0; i < sampleCount; i++) {
        qint64 offset = bitOffset + i*sampleSize;
        if (offset+sampleSize >= container->bits()->sizeInBits()) {
            buffer[i][0] = 0.0;
            buffer[i][1] = 0.0;
            continue;
        }
        if (m_wordFormat == TwosComplement) {
            buffer[i][0] = double(container->bits()->getWordValueTwosComplement(offset, m_wordSize)) * wordInverse;
        }
        else {
            buffer[i][0] = double(container->bits()->getWordValue(offset, m_wordSize)) * wordInverse;
        }
        if (withComplex) {
            if (m_wordFormat == TwosComplement) {
                buffer[i][1] = double(container->bits()->getWordValueTwosComplement(offset + m_wordSize, m_wordSize)) * wordInverse;
            }
            else {
                buffer[i][1] = double(container->bits()->getWordValue(offset + m_wordSize, m_wordSize)) * wordInverse;
            }
        }
        else {
            buffer[i][1] = 0.0;
        }
    }
}

QList<QVector<double>> SpectrogramWidget::computeStft(int maxSpectrums, qint64 bitOffset, QSharedPointer<BitContainer> container)
{
    QList<QVector<double>> spectrums;
    if (m_wordSize > 64 || m_wordSize < 1) {
        return spectrums;
    }

    fftw_complex *fftIn = reinterpret_cast<fftw_complex*>(fftw_malloc(sizeof(fftw_complex) * unsigned(m_fftSize)));
    fftw_complex *fftOut = reinterpret_cast<fftw_complex*>(fftw_malloc(sizeof(fftw_complex) * unsigned(m_fftSize)));
    fftw_plan plan = fftw_plan_dft_1d(m_fftSize, fftIn, fftOut, FFTW_FORWARD, FFTW_ESTIMATE);

    double outputFactor = 2.0 / double(m_fftSize);

    int fftBits = m_fftSize*m_wordSize;
    int strideBits = bitStride();

    for (int i = 0; i < maxSpectrums; i++) {
        if (bitOffset + fftBits >= container->bits()->sizeInBits()) {
            break;
        }
        fillSamples(fftIn, m_fftSize, bitOffset, container);
        fftw_execute_dft(plan, fftIn, fftOut);

        QVector<double> spectrum(m_fftSize/2);
        for (int n = 0; n < m_fftSize/2; n++) {
            spectrum[n] = (fftOut[n][0] * fftOut[n][0] * outputFactor) + (fftOut[n][1] * fftOut[n][1] * outputFactor);
        }
        spectrums.append(spectrum);
        bitOffset += strideBits;
    }

    fftw_destroy_plan(plan);

    fftw_free(fftIn);
    fftw_free(fftOut);

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


