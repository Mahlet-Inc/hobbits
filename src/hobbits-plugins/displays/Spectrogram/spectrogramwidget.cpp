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
    m_displayOffset(0, 0),
    m_headerFontSize(0, 0),
    m_renderer(new SpectrogramRenderer(this))
{
    connect(m_renderer, &SpectrogramRenderer::spectrumsChanged, [this](const QList<QVector<double>> &s, const QImage &img) {
        m_spectrogram = img;
        m_spectrums = s;
        repaint();
    });
}

void SpectrogramWidget::paintEvent(QPaintEvent*) {
    if (m_displayHandle->getContainer().isNull()) {
        return;
    }

    m_renderer->setContainer(m_displayHandle->getContainer());

    int frameOffset = m_displayHandle->getFrameOffset();
    if (frameOffset < 0 || frameOffset >= m_displayHandle->getContainer()->frames().size()) {
        return;
    }
    qint64 bitOffset = m_displayHandle->getContainer()->frames().at(frameOffset).start();
    m_renderer->setBitOffset(bitOffset);

    prepareHeaders();

    int displayWidth = (this->width() - m_displayOffset.x()) / m_scale;
    int displayHeight = (this->height() - m_displayOffset.y()) / m_scale;

    m_renderer->setMaxSpectrums(displayHeight);

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
        qint64 sampleOffset = bitOffset / m_renderer->wordSize();
        if (m_renderer->dataType() == SpectrogramRenderer::RealComplexInterleaved) {
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

            qint64 sample = i * m_renderer->fftSize();
            if (m_renderer->overlap() > 1) {
                sample /= m_renderer->overlap();
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
            double freq = percent * m_renderer->sampleRate() / 2.0;
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


//    auto spectrums = computeStft(displayHeight, bitOffset, m_displayHandle->getContainer());

//    QColor c = SettingsManager::getInstance().getUiSetting(SettingsData::BYTE_HUE_SAT_KEY).value<QColor>();
//    qreal hue = c.hueF();
//    qreal saturation = c.saturationF();
//    QImage raster(m_fftSize/2, displayHeight, QImage::Format_ARGB32);
//    raster.fill(qRgba(0x00, 0x00, 0x00, 0xff));
//    for (int y = 0; y < spectrums.size(); y++) {
//        auto spectrum = spectrums.at(y);
//        for (int x = 0; x < m_fftSize/2  && x < spectrum.size(); x++) {
//            qreal lightness = qBound(0.0, spectrum.at(x)*m_sensitivity, 1.0);
//            c.setHslF(hue, saturation, lightness);
//            raster.setPixel(x, y, c.rgba());
//        }
//    }

    painter.setRenderHint(QPainter::Antialiasing, false);
    painter.drawImage(QRect(m_displayOffset.x(), m_displayOffset.y(), displayWidth*m_scale, displayHeight*m_scale), m_spectrogram);
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
    int bitsPerY = m_renderer->bitStride();
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
    m_renderer->setOverlap(val);
}

void SpectrogramWidget::setFftSize(int val)
{
    m_renderer->setFftSize(val);
}

void SpectrogramWidget::setWordSize(int val)
{
    m_renderer->setWordSize(val);
}

void SpectrogramWidget::setWordFormat(int val)
{
    m_renderer->setWordFormat(val);
}

void SpectrogramWidget::setDataType(int val)
{
    m_renderer->setDataType(static_cast<SpectrogramRenderer::DataType>(val));
}

void SpectrogramWidget::setSensitivity(double val)
{
    m_renderer->setSensitivity(val);
}

void SpectrogramWidget::setSampleRate(double val)
{
    m_renderer->setSampleRate(val);
}

void SpectrogramWidget::setShowHeaders(bool val)
{
    m_showFrameOffsets = val;
    m_showColumnOffsets = val;
    repaint();
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
    double seconds = double(sample) / double(m_renderer->sampleRate());
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


