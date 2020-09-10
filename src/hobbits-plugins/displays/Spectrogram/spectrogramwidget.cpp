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
    m_showHoverSlices(false),
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
    if (frameOffset < 0 || frameOffset >= m_displayHandle->getContainer()->info()->frames()->size()) {
        return;
    }
    qint64 bitOffset = m_displayHandle->getContainer()->info()->frames()->at(frameOffset).start();
    m_renderer->setBitOffset(bitOffset);

    prepareHeaders();

    int displayWidth = m_displayCenterSize.width() / m_scale;
    int displayHeight = m_displayCenterSize.height() / m_scale;

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
    qint64 sampleOffset = bitOffset / m_renderer->wordSize();
    if (m_renderer->dataType() == SpectrogramRenderer::RealComplexInterleaved) {
        sampleOffset /= 2;
    }
    if (m_showFrameOffsets) {
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
                    getFreq(percent));
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

    if (m_hoverX >= 0 && m_hoverY >= 0) {
        painter.save();
        painter.setCompositionMode(QPainter::RasterOp_SourceXorDestination);
        painter.setPen(QColor(0xff, 0xff, 0xff));

        int x = m_hoverX * m_scale;
        int y = m_hoverY * m_scale;

        painter.translate(m_displayOffset);
        painter.drawLine(0, y, m_displayCenterSize.width(), y);
        painter.drawLine(x, 0, x, m_displayCenterSize.height());

        QString freq = getFreq(double(m_hoverX) / double(m_displayCenterSize.width()));

        qint64 hoverSample = m_hoverY * m_renderer->fftSize();
        if (m_renderer->overlap() > 1) {
            hoverSample /= m_renderer->overlap();
        }
        hoverSample += sampleOffset;

        QString time = timeString(hoverSample);


        int pad = 4;
        int textHeight = m_headerFontSize.height() + 3;
        int boxWidth = m_headerFontSize.width() * qMax(time.size(), freq.size()) + pad*2;
        int boxHeight = textHeight * 2 + pad*2;

        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);

        QRect box;
        if (x > m_displayCenterSize.width() / 2) {
            box.setX(x - pad - boxWidth);
        }
        else {
            box.setX(x + pad);
        }
        if (y > m_displayCenterSize.height() / 2) {
            box.setY(y - pad - boxHeight);
        }
        else {
            box.setY(y + pad);
        }
        box.setWidth(boxWidth);
        box.setHeight(boxHeight);

        painter.fillRect(box, QColor(0x00, 0x00, 0x00, 0x99));

        painter.setFont(font);
        painter.drawText(box.x() + pad, box.y() + pad, box.width(), textHeight, Qt::AlignLeft, freq);
        painter.drawText(box.x() + pad, box.y() + pad + textHeight, box.width(), textHeight, Qt::AlignLeft, time);

        painter.restore();
    }

    if (m_showHoverSlices) {
        painter.save();

        painter.translate(m_displayOffset);

        int borderSize = 4;


        QRect rBorder;
        rBorder.setTopLeft(QPoint(m_displayCenterSize.width(), 0));
        rBorder.setWidth(borderSize);
        rBorder.setHeight(this->height() - m_displayOffset.y());

        QRect bBorder;
        bBorder.setTopLeft(QPoint(0, m_displayCenterSize.height()));
        bBorder.setWidth(this->width() - m_displayOffset.x());
        bBorder.setHeight(borderSize);

        QRect rPanel;
        rPanel.setTopLeft(QPoint(m_displayCenterSize.width() + borderSize, 0));
        rPanel.setBottomRight(QPoint(this->width() - m_displayOffset.x(), this->height() - m_displayOffset.y()));
        QRect bPanel;
        bPanel.setTopLeft(QPoint(0, m_displayCenterSize.height() + borderSize));
        bPanel.setBottomRight(rPanel.bottomRight());

        painter.fillRect(rPanel, Qt::black);
        painter.fillRect(bPanel, Qt::black);
        painter.fillRect(rBorder, Qt::darkGray);
        painter.fillRect(bBorder, Qt::darkGray);

        int graphPad = 8;

        if (m_hoverX >= 0 && m_hoverY >= 0) {
            if (m_hoverY < m_spectrums.size() && m_spectrums.size() > 0) {
                painter.save();
                painter.translate(bPanel.bottomLeft() + QPoint(0, -1*graphPad));
                painter.scale(1, -1.0);
                auto spectrum = m_spectrums.at(m_hoverY);


                double yFactor = bPanel.height() - (2 * graphPad);
                double xFactor = double(m_displayCenterSize.width()) / (double(m_renderer->fftSize()) / 2.0);
                QPainterPath path;
                path.moveTo(0, qBound(0.0, spectrum.at(0)*m_renderer->sensitivity(), 1.0)*yFactor);
                double xVal = xFactor;
                for (int i = 1; i < spectrum.size(); i++, xVal += xFactor) {
                    path.lineTo(xVal, qBound(0.0, spectrum.at(i)*m_renderer->sensitivity(), 1.0)*yFactor);
                }

                painter.setRenderHint(QPainter::Antialiasing, true);
                painter.setPen(qRgb(0xdd, 0xdd, 0xdd));
                painter.drawPath(path);

                painter.restore();
            }

            if (m_spectrums.size() > 0) {
                painter.save();
                painter.translate(rPanel.topLeft() + QPoint(graphPad, 0));
                painter.rotate(90);
                painter.scale(1.0, -1.0);

                int pos = qFloor(double(m_hoverX) / double(m_displayCenterSize.width()) * (double(m_renderer->fftSize()) / 2.0));


                double xFactor = m_scale;
                double yFactor = rPanel.width() - (2 * graphPad);
                QPainterPath path;
                path.moveTo(0, qBound(0.0, m_spectrums.at(0).at(pos)*m_renderer->sensitivity(), 1.0)*yFactor);
                double xVal = xFactor;
                for (int i = 1; i < m_spectrums.size(); i++, xVal += xFactor) {
                    path.lineTo(xVal, qBound(0.0, m_spectrums.at(i).at(pos)*m_renderer->sensitivity(), 1.0)*yFactor);
                }

                painter.setRenderHint(QPainter::Antialiasing, true);
                painter.setPen(qRgb(0xdd, 0xdd, 0xdd));
                painter.drawPath(path);


                painter.restore();
            }
        }

        painter.restore();
    }
}

void SpectrogramWidget::leaveEvent(QEvent *event)
{
    DisplayBase::leaveEvent(event);
    m_hoverY = -1;
    m_hoverX = -1;
    repaint();
}

void SpectrogramWidget::mouseMoveEvent(QMouseEvent *event) {
    bool wasHovering = (m_hoverX >= 0 || m_hoverY >=0);
    m_hoverY = -1;
    m_hoverX = -1;

    if (m_displayHandle->getContainer().isNull()) {
        emit bitHover(false, 0, 0);
        if (wasHovering) {
            repaint();
        }
        return;
    }


    int y = (event->y() - m_displayOffset.y());
    int x = (event->x() - m_displayOffset.x());
    if (y < 0 || x < 0 || y >= m_displayCenterSize.height() || x >= m_displayCenterSize.width()) {
        emit bitHover(false, 0, 0);
        if (wasHovering) {
            repaint();
        }
        return;
    }

    y /= m_scale;
    x /= m_scale;

    int baseFrameOffset = m_displayHandle->getFrameOffset();
    if (baseFrameOffset < 0 || baseFrameOffset > m_displayHandle->getContainer()->frameCount()) {
        emit bitHover(false, 0, 0);
        if (wasHovering) {
            repaint();
        }
        return;
    }
    qint64 baseBitOffset = m_displayHandle->getContainer()->frameAt(baseFrameOffset).start();
    int bitsPerY = m_renderer->bitStride();
    qint64 bitOffset = y * bitsPerY + baseBitOffset;
    if (bitOffset >= m_displayHandle->getContainer()->bits()->sizeInBits()) {
        emit bitHover(false, 0, 0);
        if (wasHovering) {
            repaint();
        }
        return;
    }
    int frameOffset = m_displayHandle->getContainer()->info()->frameOffsetContaining(bitOffset);

    emit bitHover(event, 0, frameOffset);

    m_hoverY = y;
    m_hoverX = x;

    repaint();
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

void SpectrogramWidget::setShowHoverSlices(bool val)
{
    m_showHoverSlices = val;
    repaint();
}

void SpectrogramWidget::setLogarithmic(bool val)
{
    m_renderer->setLogarithmic(val);
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

    if (m_showHoverSlices) {
        m_displayCenterSize.setWidth(int(double(this->width() - m_displayOffset.x()) * 0.90));
        m_displayCenterSize.setHeight(int(double(this->height() - m_displayOffset.y()) * 0.80));
    }
    else {
        m_displayCenterSize.setWidth(this->width() - m_displayOffset.x());
        m_displayCenterSize.setHeight(this->height() - m_displayOffset.y());
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

QString SpectrogramWidget::getFreq(double percent)
{
    double freq = percent * m_renderer->sampleRate() / 2.0;
    QStringList units = {"Hz", "kHz", "MHz", "GHz", "THz"};
    int unitIndex = 0;
    while (freq >= 1000.0 && unitIndex + 1 < units.size()) {
        unitIndex++;
        freq /= 1000.0;
    }

    return QString("%1 %2").arg(freq, 0, 'f', 2).arg(units.at(unitIndex));
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


