#include "spectrogramwidget.h"

#include <QPainter>
#include <QImage>
#include "displayhelper.h"
#include "fftw3.h"
#include <QtMath>
#include <QMouseEvent>
#include "settingsmanager.h"
#include <QTime>

#include <QDebug>

SpectrogramWidget::SpectrogramWidget(
        QSharedPointer<DisplayHandle> displayHandle,
        DisplayInterface *pluginRef,
        QWidget *parent) :
    DisplayBase(displayHandle, pluginRef, parent),
    m_scale(1),
    m_showFrameOffsets(false),
    m_showColumnOffsets(true),
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
    qint64 bitOffset = m_displayHandle->getContainer()->frames().at(frameOffset).start();

    prepareHeaders();

    int displayWidth = (this->width() - m_displayOffset.x()) / m_scale;
    int displayHeight = (this->height() - m_displayOffset.y()) / m_scale;

    QFont font = QFont("monospace", 10);
    font.setStyleStrategy(QFont::ForceIntegerMetrics);

    QPainter painter(this);
    if (m_showFrameOffsets) {
        painter.fillRect(0, 0, m_displayOffset.x(), height(), Qt::lightGray);
    }
    if (m_showColumnOffsets) {
        painter.fillRect(0, 0, width(), m_displayOffset.y(), Qt::lightGray);
    }
    painter.save();
    if (m_showFrameOffsets) {
        qint64 sampleOffset = bitOffset / m_wordSize;
        if (m_dataType == RealComplexInterleaved) {
            sampleOffset /= 2;
        }
        int increment = qCeil(double(m_headerFontSize.height()) / double(m_scale));
        for (int i = 0; i <= displayHeight; i += increment) {
            if (increment <= 0) {
                break;
            }
            int yOffset = -2;

            if (increment > 1) {
                yOffset = -1 * qRound(double(m_headerFontSize.height() - m_scale) / 2.0);
                painter.fillRect(
                        m_displayOffset.x() - m_headerFontSize.width() / 2,
                        m_displayOffset.y() + (i * m_scale),
                        m_headerFontSize.width() / 2,
                        m_scale,
                        Qt::darkGray);
            }

            qint64 sample = i * m_fftSize;
            if (m_overlap > 1) {
                sample /= m_overlap;
            }
            sample += sampleOffset;

            painter.setPen(Qt::darkGray);
            painter.setFont(font);
            painter.drawText(
                    m_headerFontSize.width() / 2,
                    m_displayOffset.y() + (i * m_scale) + yOffset,
                    m_displayOffset.x() - m_headerFontSize.width(),
                    m_headerFontSize.height(),
                    Qt::AlignRight | Qt::AlignTop,
                    timeString(sample));
        }
    }
    painter.restore();
    painter.save();
    if (m_showColumnOffsets) {
        int increment = qCeil(double(m_headerFontSize.height()) / double(m_scale));
        for (int i = 0; i < displayWidth; i += increment) {
            if (increment <= 0) {
                break;
            }

            int yOffset = -2;

            double percent = double(i) / double(displayWidth);
            double freq = percent * m_sampleRate / 2.0;
            QStringList units = {"Hz", "kHz", "MHz", "GHz", "THz"};
            int unitIndex = 0;
            while (freq >= 1000.0 && unitIndex + 1 < units.size()) {
                unitIndex++;
                freq /= 1000.0;
            }


            painter.save();
            painter.rotate(-90);
            painter.setPen(Qt::darkGray);
            painter.setFont(font);
            painter.drawText(
                    -1 * m_displayOffset.y() + m_headerFontSize.width() / 2,
                    m_displayOffset.x() + i * m_scale + yOffset,
                    m_displayOffset.y() - m_headerFontSize.width(),
                    m_headerFontSize.height(),
                    Qt::AlignLeft,
                    QString("%1 %2").arg(freq, 0, 'f', 2).arg(units.at(unitIndex)));
            painter.restore();
        }
    }
    painter.restore();


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
    m_wordFormat = val;
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

void SpectrogramWidget::setSampleRate(double val)
{
    m_sampleRate = val;
    repaint();
}

void SpectrogramWidget::setShowHeaders(bool val)
{
    m_showFrameOffsets = val;
    m_showColumnOffsets = val;
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
    bool littleEndian = (m_wordFormat & LittleEndian);
    for (int i = 0; i < sampleCount; i++) {
        qint64 offset = bitOffset + i*sampleSize;
        if (offset+sampleSize >= container->bits()->sizeInBits()) {
            buffer[i][0] = 0.0;
            buffer[i][1] = 0.0;
            continue;
        }
        if (m_wordFormat & TwosComplement) {
            buffer[i][0] = double(container->bits()->getWordValueTwosComplement(offset, m_wordSize, littleEndian)) * wordInverse;
        }
        else {
            buffer[i][0] = double(container->bits()->getWordValue(offset, m_wordSize, littleEndian)) * wordInverse;
        }
        if (withComplex) {
            if (m_wordFormat & TwosComplement) {
                buffer[i][1] = double(container->bits()->getWordValueTwosComplement(offset + m_wordSize, m_wordSize, littleEndian)) * wordInverse;
            }
            else {
                buffer[i][1] = double(container->bits()->getWordValue(offset + m_wordSize, m_wordSize, littleEndian)) * wordInverse;
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
        int maxChars = 12;
        m_displayOffset.setX(qRound(fontWidth * (maxChars + 1.5)));
    }
    else {
        m_displayOffset.setX(0);
    }

    if (m_showColumnOffsets) {
        int maxChars = 10;
        m_displayOffset.setY(fontWidth * (maxChars + 1));
    }
    else {
        m_displayOffset.setY(0);
    }
}

QString SpectrogramWidget::timeString(qint64 sample)
{
    double seconds = double(sample) / double(m_sampleRate);
    if (seconds < 1.0e-4) {
        int ns = int(seconds * 1.0e9);
        return QString("%1 ns").arg(ns);
    }
    else if (seconds < 1.0) {
        double ms = seconds * 1.0e3;
        return QString("%1 ms").arg(ms, 0, 'f', 3);
    }
    else if (seconds < 60.0) {
        return QString("%1 s").arg(seconds, 0, 'f', 3);
    }
    else {
        int ms = int(qRound(seconds * 1.0e3));
        int s = ms / 1000;
        int m = s / 60;
        int h = m / 60;
        return QString("%1:%2:%3.%4")
                .arg(h)
                .arg(m % 60, 2, 10, QChar('0'))
                .arg(s % 60, 2, 10, QChar('0'))
                .arg((ms % 1000) / 10, 2, 10, QChar('0'));
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


