#include "spectrogram.h"
#include "displayhelper.h"
#include <QPainter>
#include "math.h"
#include <QTime>
#include "viridis.h"
#include <QtMath>

Spectrogram::Spectrogram():
    m_renderConfig(new DisplayRenderConfig())
{
    m_renderConfig->setAsynchronous(true);
    m_renderConfig->setHideBitOffsetControls(true);
    m_renderConfig->setFullRedrawTriggers(DisplayRenderConfig::NewFrameOffset);
    m_renderConfig->setOverlayRedrawTriggers(DisplayRenderConfig::NewMouseHover);

    QList<ParameterDelegate::ParameterInfo> infos = {
        {"sample_format", QJsonValue::String},
        {"fft_overlap", QJsonValue::Double},
        {"fft_size", QJsonValue::Double},
        {"data_type", QJsonValue::Double},
        {"sensitivity", QJsonValue::Double},
        {"sample_rate", QJsonValue::Double},
        {"show_headers", QJsonValue::Bool},
        {"show_slices", QJsonValue::Bool},
        {"logarithmic_scaling", QJsonValue::Bool}
    };

    m_delegate = ParameterDelegate::create(
                    infos,
                    [](const QJsonObject &parameters) {
                        int fftSize = parameters.value("fft_size").toInt();
                        QString format = parameters.value("sample_format").toString();
                        return QString("Spectrogram %1 %2").arg(fftSize).arg(format);
                    },
                    [](QSharedPointer<ParameterDelegate> delegate, QSize size) {
                        Q_UNUSED(size)
                        return new SpectrogramControls(delegate);
                    });
}

DisplayInterface* Spectrogram::createDefaultDisplay()
{
    return new Spectrogram();
}

QString Spectrogram::name()
{
    return "Spectrogram";
}

QString Spectrogram::description()
{
    return "Displays a power spectral density waterfall for sample-based data";
}

QStringList Spectrogram::tags()
{
    return {"DSP"};
}

QSharedPointer<DisplayRenderConfig> Spectrogram::renderConfig()
{
    return m_renderConfig;
}

void Spectrogram::setDisplayHandle(QSharedPointer<DisplayHandle> displayHandle)
{
    m_handle = displayHandle;
}

QSharedPointer<ParameterDelegate> Spectrogram::parameterDelegate()
{
    return m_delegate;
}

QImage Spectrogram::renderDisplay(QSize viewportSize, const QJsonObject &parameters, QSharedPointer<PluginActionProgress> progress)
{
    if (!m_delegate->validate(parameters)) {
        return QImage();
    }
    QRect spectRect = spectrogramRectangle(viewportSize, m_handle, parameters);
    if (spectRect.isNull()) {
        return QImage();
    }

    int fftSize = parameters.value("fft_size").toInt();
    int fftOverlap = parameters.value("fft_overlap").toInt();
    auto sampleFormat = MetadataHelper::sampleFormat(parameters.value("sample_format").toString());
    auto dataType = static_cast<SpectrogramControls::DataType>(parameters.value("data_type").toInt());
    qint64 bitOffset = m_handle->currentContainer()->info()->frames()->at(m_handle->frameOffset()).start();

    QList<QVector<double>> spectrums;
    QImage img(fftSize/2, spectRect.height(), QImage::Format_ARGB32);
    img.fill(Qt::transparent);

    if (sampleFormat.wordSize > 64 || sampleFormat.wordSize < 1) {
        return QImage();
    }

    fftw_complex *fftIn = reinterpret_cast<fftw_complex*>(fftw_malloc(sizeof(fftw_complex) * unsigned(fftSize)));
    fftw_complex *fftOut = reinterpret_cast<fftw_complex*>(fftw_malloc(sizeof(fftw_complex) * unsigned(fftSize)));
    fftw_plan plan = fftw_plan_dft_1d(fftSize, fftIn, fftOut, FFTW_FORWARD, FFTW_ESTIMATE);

    QVector<double> hanningWindow(fftSize);
    for (int i = 0; i < fftSize; i++) {
        hanningWindow[i] = 0.5*(1.0-cos(2.0*M_PI*i/double(fftSize-1.0)));
    }

    double outputFactor = 1.0 / double(fftSize);
    double sensitivity = parameters.value("sensitivity").toDouble();
    bool logarithmicScaling = parameters.value("logarithmic_scaling").toBool();
    if (logarithmicScaling) {
        outputFactor *= pow(2.0, sensitivity);
    }
    else {
        if (sensitivity < 0) {
            outputFactor *= (-1.0 / sensitivity);
        }
        else if (sensitivity > 0) {
            outputFactor *= sensitivity;
        }
    }

    int fftBits = fftSize * sampleFormat.wordSize;
    int strideBits = fftBits;
    if (fftOverlap > 0) {
        strideBits /= fftOverlap;
    }

    qint64 currOffset = bitOffset;
    currOffset /= fftBits;
    currOffset *= fftBits;

    auto container = m_handle->currentContainer();

    QTime lastTime = QTime::currentTime();
    for (int i = 0; i < spectRect.height(); i++) {
        if (!progress.isNull() && progress->isCancelled()) {
            fftw_destroy_plan(plan);
            fftw_free(fftIn);
            fftw_free(fftOut);
            return QImage();
        }

        if (currOffset + fftBits >= container->bits()->sizeInBits()) {
            break;
        }
        fillSamples(fftIn, currOffset, container, sampleFormat, fftSize, dataType);

        for (int i = 0; i < fftSize; i++) {
            fftIn[i][0] *= hanningWindow[i];
            fftIn[i][1] *= hanningWindow[i];
        }
        fftw_execute_dft(plan, fftIn, fftOut);

        QVector<double> spectrum(fftSize/2);
        if (logarithmicScaling) {
            for (int n = 0; n < spectrum.size(); n++) {
                spectrum[n] = log(outputFactor * ((fftOut[n][0] * fftOut[n][0]) + (fftOut[n][1] * fftOut[n][1]))) / log(10);
            }
        }
        else {
            for (int n = 0; n < spectrum.size(); n++) {
                spectrum[n] = outputFactor * ((fftOut[n][0] * fftOut[n][0]) + (fftOut[n][1] * fftOut[n][1]));
            }
        }
        spectrums.append(spectrum);
        for (int x = 0; x < img.width()  && x < spectrum.size(); x++) {
            img.setPixel(x, i, VIRIDIS_MAP[qBound(0, qFloor(spectrum.at(x) * 256.0), 255)].rgb());
        }

        currOffset += strideBits;

        if (!progress.isNull() && lastTime.msecsTo(QTime::currentTime()) > 400 && !progress->isCancelled()) {
            setSpectrums(spectrums);
            QImage preview(viewportSize, QImage::Format_ARGB32);
            preview.fill(Qt::transparent);
            QPainter previewPaint(&preview);
            previewPaint.drawImage(spectRect, img);

            progress->sendUpdate("image_preview", QVariant(preview));
            lastTime = QTime::currentTime();
        }
    }

    fftw_destroy_plan(plan);
    fftw_free(fftIn);
    fftw_free(fftOut);

    setSpectrums(spectrums);
    QImage result(viewportSize, QImage::Format_ARGB32);
    result.fill(Qt::transparent);
    QPainter previewPaint(&result);
    previewPaint.drawImage(spectRect, img);

    return result;
}

QImage Spectrogram::renderOverlay(QSize viewportSize, const QJsonObject &parameters)
{
    if (!m_delegate->validate(parameters)) {
        return QImage();
    }

    QRect spectRect = spectrogramRectangle(viewportSize, m_handle, parameters);
    if (spectRect.isNull()) {
        return QImage();
    }

    QImage overlay(viewportSize, QImage::Format_ARGB32);
    overlay.fill(Qt::transparent);
    QPainter painter(&overlay);

    QFont font = DisplayHelper::monoFont(10);
    QSize fontSize = DisplayHelper::textSize(font, "0");

    painter.setPen(Qt::darkGray);
    painter.setFont(font);

    auto sampleFormat = MetadataHelper::sampleFormat(parameters.value("sample_format").toString());
    auto dataType = static_cast<SpectrogramControls::DataType>(parameters.value("data_type").toInt());
    qint64 bitOffset = m_handle->currentContainer()->info()->frames()->at(m_handle->frameOffset()).start();
    qint64 sampleOffset = bitOffset / sampleFormat.wordSize;
    if (dataType == SpectrogramControls::RealComplexInterleaved) {
        sampleOffset /= 2;
    }

    QStringList freqUnits = {"Hz", "kHz", "MHz", "GHz", "THz"};
    int fftSize = parameters.value("fft_size").toInt();
    int overlap = parameters.value("fft_overlap").toInt();
    double sampleRate = parameters.value("sample_rate").toDouble();
    // Draw headers
    if (spectRect.x() > 0) {
        painter.save();
        painter.fillRect(0, 0, viewportSize.width(), spectRect.y(), DisplayHelper::headerBackgroundColor());
        painter.fillRect(0, 0, spectRect.x(), viewportSize.height(), DisplayHelper::headerBackgroundColor());

        int yOffset = -1 * qRound(double(fontSize.height()) / 2.0);
        for (int i = 0; i <= spectRect.height(); i += fontSize.height()) {
            painter.fillRect(
                    spectRect.x() - fontSize.width() / 2,
                    spectRect.y() + i,
                    fontSize.width() / 2,
                    1,
                    Qt::darkGray);

            qint64 sample = i * fftSize;
            if (overlap > 1) {
                sample /= overlap;
            }
            sample += sampleOffset;

            painter.drawText(
                    fontSize.width() / 2,
                    spectRect.y() + i + yOffset,
                    spectRect.x() - fontSize.width(),
                    fontSize.height(),
                    Qt::AlignRight | Qt::AlignTop,
                    timeString(sample, sampleRate));
        }

        painter.rotate(-90);
        for (int i = 0; i < spectRect.width(); i += fontSize.height()) {

            double percent = double(i) / double(spectRect.width());
            double freq = percent * sampleRate / 2.0;
            int unitIndex = 0;
            while (freq >= 1000.0 && unitIndex + 1 < freqUnits.size()) {
                unitIndex++;
                freq /= 1000.0;
            }
            QString freqString = QString("%1 %2").arg(freq, 0, 'f', 2).arg(freqUnits.at(unitIndex));

            painter.drawText(
                    -1 * spectRect.y() + fontSize.width() / 2,
                    spectRect.x() + i + yOffset,
                    spectRect.y() - fontSize.width(),
                    fontSize.height(),
                    Qt::AlignLeft,
                    freqString);
        }

        painter.restore();
    }

    QPoint mouseHover = m_handle->mouseHover(this);

    // draw hover details
    if (!mouseHover.isNull() && spectRect.contains(mouseHover)) {
        int x = mouseHover.x() - spectRect.x();
        int y = mouseHover.y() - spectRect.y();

        // draw crosshairs
        painter.save();
        painter.setPen(QColor(0x00, 0x00, 0x00, 0x88));
        painter.translate(spectRect.topLeft());
        painter.drawLine(0, y, spectRect.width(), y);
        painter.drawLine(x, 0, x, spectRect.height());
        painter.restore();

        // draw info box
        double percent = double(x) / double(spectRect.width());
        double freq = percent * sampleRate / 2.0;
        int unitIndex = 0;
        while (freq >= 1000.0 && unitIndex + 1 < freqUnits.size()) {
            unitIndex++;
            freq /= 1000.0;
        }
        QString freqString = QString("%1 %2").arg(freq, 0, 'f', 2).arg(freqUnits.at(unitIndex));

        qint64 hoverSample = y * fftSize;
        if (overlap > 1) {
           hoverSample /= overlap;
        }
        hoverSample += sampleOffset;
        QString time = timeString(hoverSample, sampleRate);

        DisplayHelper::drawHoverBox(&painter, spectRect, mouseHover, {freqString, time});
    }

    // Draw slices
    if (spectRect.right() < viewportSize.width()) {
        painter.save();

        painter.translate(spectRect.topLeft());

        int borderSize = 4;

        QRect rPanel;
        rPanel.setTopLeft(QPoint(spectRect.width() + borderSize, 0));
        rPanel.setBottomRight(QPoint(viewportSize.width() - spectRect.x(), spectRect.height()));
        QRect bPanel;
        bPanel.setTopLeft(QPoint(0, spectRect.height() + borderSize));
        bPanel.setBottomRight(QPoint(spectRect.width(), viewportSize.height() - spectRect.y()));

        painter.fillRect(rPanel, QColor(0x1c, 0x1c, 0x1c));
        painter.fillRect(bPanel, QColor(0x1c, 0x1c, 0x1c));

        int graphPad = 8;

        if (!mouseHover.isNull() && spectRect.contains(mouseHover)) {
            int x = mouseHover.x() - spectRect.x();
            int y = mouseHover.y() - spectRect.y();

            if (y < m_spectrums.size() && m_spectrums.size() > 0) {
                painter.save();
                painter.translate(bPanel.bottomLeft() + QPoint(0, -1*graphPad));
                painter.scale(1, -1.0);
                auto spectrum = m_spectrums.at(y);

                double yFactor = bPanel.height() - (2 * graphPad);
                double xFactor = double(spectRect.width()) / (double(fftSize) / 2.0);
                QPainterPath path;
                path.moveTo(0, qBound(0.0, spectrum.at(0), 1.0)*yFactor);
                double xVal = xFactor;
                for (int i = 1; i < spectrum.size(); i++, xVal += xFactor) {
                    path.lineTo(xVal, qBound(0.0, spectrum.at(i), 1.0)*yFactor);
                }

                painter.setRenderHint(QPainter::Antialiasing, true);
                painter.setPen(qRgb(0xa3, 0xbe, 0xdb));
                painter.drawPath(path);

                painter.restore();
            }

            if (m_spectrums.size() > 0) {
                painter.save();
                painter.translate(rPanel.topLeft() + QPoint(graphPad, 0));
                painter.rotate(90);
                painter.scale(1.0, -1.0);

                int pos = qFloor(double(x) / double(spectRect.width()) * (double(fftSize) / 2.0));

                double xFactor = 1;
                double yFactor = rPanel.width() - (2 * graphPad);
                QPainterPath path;
                path.moveTo(0, qBound(0.0, m_spectrums.at(0).at(pos), 1.0)*yFactor);
                double xVal = xFactor;
                for (int i = 1; i < m_spectrums.size(); i++, xVal += xFactor) {
                    path.lineTo(xVal, qBound(0.0, m_spectrums.at(i).at(pos), 1.0)*yFactor);
                }

                painter.setRenderHint(QPainter::Antialiasing, true);
                painter.setPen(qRgb(0xa3, 0xbe, 0xdb));
                painter.drawPath(path);


                painter.restore();
            }
        }

        painter.restore();
    }

    if (!m_handle->currentContainer().isNull() && m_handle->currentContainer()->frameCount() > 0) {
        qint64 lastSample = spectRect.height() * fftSize;
        if (overlap > 1) {
            lastSample /= overlap;
        }
        lastSample += sampleOffset;
        qint64 lastVisibleBit = lastSample * sampleFormat.wordSize;
        lastVisibleBit = qMin(m_handle->currentContainer()->frameAt(m_handle->currentContainer()->frameCount() - 1).end(), lastVisibleBit);
        qint64 frameOffset = m_handle->currentContainer()->info()->frameOffsetContaining(lastVisibleBit);
        DisplayHelper::setRenderRange(this, m_handle, int(frameOffset - m_handle->frameOffset()));
    }

    return overlay;
}

QRect Spectrogram::spectrogramRectangle(QSize viewportSize, QSharedPointer<DisplayHandle> displayHandle, const QJsonObject &parameters)
{
    if (displayHandle->currentContainer().isNull()) {
        return QRect();
    }

    QFont font = DisplayHelper::monoFont(10);
    QSize fontSize = DisplayHelper::textSize(font, "0");
    QPoint headerOffset(0, 0);
    if (parameters.value("show_headers").toBool()) {
        headerOffset.setX(qRound(13.5 * fontSize.width()));
        headerOffset.setY(10 * fontSize.width());
    }

    QSize displaySize(viewportSize.width() - headerOffset.x(), viewportSize.height() - headerOffset.y());

    if (parameters.value("show_slices").toBool()) {

        int sliceMargin = qFloor(qMin(double(displaySize.width()) * 0.1, double(displaySize.height()) * 0.15));

        displaySize.setWidth(displaySize.width() - sliceMargin);
        displaySize.setHeight(displaySize.height() - sliceMargin);
    }

    return QRect(headerOffset, displaySize);
}

QString Spectrogram::timeString(qint64 sample, double sampleRate)
{
    double seconds = double(sample) / sampleRate;
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

void Spectrogram::fillSamples(fftw_complex* buffer,
                              qint64 offset,
                              QSharedPointer<BitContainer> bitContainer,
                              const MetadataHelper::SampleFormat &sampleFormat,
                              int fftSize,
                              SpectrogramControls::DataType dataType)
{
    offset /= sampleFormat.wordSize;
    qint64 maxSamples = fftSize;
    if (dataType == SpectrogramControls::RealComplexInterleaved) {
        maxSamples *= 2;
    }
    qint64 samples = 0;
    double weight = double(1ull << (sampleFormat.wordSize - 1));
    if (sampleFormat.id.startsWith("float32")) {
        float *fBuffer = new float[maxSamples];
        samples = bitContainer->bits()->readFloat32Samples(fBuffer, offset, maxSamples, !sampleFormat.littleEndian);

        if (dataType == SpectrogramControls::RealComplexInterleaved) {
            for (int i = 0; i < samples/2; i++) {
                buffer[i][0] = double(fBuffer[i*2]);
                buffer[i][1] = double(fBuffer[i*2 + 1]);
            }
        }
        else {
            for (int i = 0; i < samples; i++) {
                buffer[i][0] = double(fBuffer[i]);
                buffer[i][1] = 0.0;
            }
        }
        delete[] fBuffer;
    }
    else if (sampleFormat.id.startsWith("float64")) {
        double *sBuffer = new double[maxSamples];
        samples = bitContainer->bits()->readFloat64Samples(sBuffer, offset, maxSamples, !sampleFormat.littleEndian);

        if (dataType == SpectrogramControls::RealComplexInterleaved) {
            for (int i = 0; i < samples/2; i++) {
                buffer[i][0] = double(sBuffer[i*2]);
                buffer[i][1] = double(sBuffer[i*2 + 1]);
            }
        }
        else {
            for (int i = 0; i < samples; i++) {
                buffer[i][0] = double(sBuffer[i]);
                buffer[i][1] = 0.0;
            }
        }
        delete[] sBuffer;
    }
    else if (sampleFormat.id.startsWith("int8")) {
        qint8 *sBuffer = new qint8[maxSamples];
        samples = bitContainer->bits()->readBytes(reinterpret_cast<char*>(sBuffer), offset, maxSamples);

        if (dataType == SpectrogramControls::RealComplexInterleaved) {
            for (int i = 0; i < samples/2; i++) {
                buffer[i][0] = double(sBuffer[i*2]) / weight;
                buffer[i][1] = double(sBuffer[i*2 + 1]) / weight;
            }
        }
        else {
            for (int i = 0; i < samples; i++) {
                buffer[i][0] = double(sBuffer[i]) / weight;
                buffer[i][1] = 0.0;
            }
        }
        delete[] sBuffer;
    }
    else if (sampleFormat.id.startsWith("uint8")) {
        quint8 *sBuffer = new quint8[maxSamples];
        samples = bitContainer->bits()->readBytes(reinterpret_cast<char*>(sBuffer), offset, maxSamples);

        if (dataType == SpectrogramControls::RealComplexInterleaved) {
            for (int i = 0; i < samples/2; i++) {
                buffer[i][0] = double(sBuffer[i*2]) / weight;
                buffer[i][1] = double(sBuffer[i*2 + 1]) / weight;
            }
        }
        else {
            for (int i = 0; i < samples; i++) {
                buffer[i][0] = double(sBuffer[i]) / weight;
                buffer[i][1] = 0.0;
            }
        }
        delete[] sBuffer;
    }
    else if (sampleFormat.id.startsWith("int16")) {
        qint16 *sBuffer = new qint16[maxSamples];
        samples = bitContainer->bits()->readInt16Samples(sBuffer, offset, maxSamples, !sampleFormat.littleEndian);

        if (dataType == SpectrogramControls::RealComplexInterleaved) {
            for (int i = 0; i < samples/2; i++) {
                buffer[i][0] = double(sBuffer[i*2]) / weight;
                buffer[i][1] = double(sBuffer[i*2 + 1]) / weight;
            }
        }
        else {
            for (int i = 0; i < samples; i++) {
                buffer[i][0] = double(sBuffer[i]) / weight;
                buffer[i][1] = 0.0;
            }
        }
        delete[] sBuffer;
    }
    else if (sampleFormat.id.startsWith("uint16")) {
        quint16 *sBuffer = new quint16[maxSamples];
        samples = bitContainer->bits()->readUInt16Samples(sBuffer, offset, maxSamples, !sampleFormat.littleEndian);

        if (dataType == SpectrogramControls::RealComplexInterleaved) {
            for (int i = 0; i < samples/2; i++) {
                buffer[i][0] = double(sBuffer[i*2]) / weight;
                buffer[i][1] = double(sBuffer[i*2 + 1]) / weight;
            }
        }
        else {
            for (int i = 0; i < samples; i++) {
                buffer[i][0] = double(sBuffer[i]) / weight;
                buffer[i][1] = 0.0;
            }
        }
        delete[] sBuffer;
    }
    else if (sampleFormat.id.startsWith("int24")) {
        qint32 *sBuffer = new qint32[maxSamples];
        samples = bitContainer->bits()->readInt24Samples(sBuffer, offset, maxSamples, !sampleFormat.littleEndian);

        if (dataType == SpectrogramControls::RealComplexInterleaved) {
            for (int i = 0; i < samples/2; i++) {
                buffer[i][0] = double(sBuffer[i*2]) / weight;
                buffer[i][1] = double(sBuffer[i*2 + 1]) / weight;
            }
        }
        else {
            for (int i = 0; i < samples; i++) {
                buffer[i][0] = double(sBuffer[i]) / weight;
                buffer[i][1] = 0.0;
            }
        }
        delete[] sBuffer;
    }
    else if (sampleFormat.id.startsWith("uint24")) {
        quint32 *sBuffer = new quint32[maxSamples];
        samples = bitContainer->bits()->readUInt24Samples(sBuffer, offset, maxSamples, !sampleFormat.littleEndian);

        if (dataType == SpectrogramControls::RealComplexInterleaved) {
            for (int i = 0; i < samples/2; i++) {
                buffer[i][0] = double(sBuffer[i*2]) / weight;
                buffer[i][1] = double(sBuffer[i*2 + 1]) / weight;
            }
        }
        else {
            for (int i = 0; i < samples; i++) {
                buffer[i][0] = double(sBuffer[i]) / weight;
                buffer[i][1] = 0.0;
            }
        }
        delete[] sBuffer;
    }
    else if (sampleFormat.id.startsWith("int32")) {
        qint32 *sBuffer = new qint32[maxSamples];
        samples = bitContainer->bits()->readInt32Samples(sBuffer, offset, maxSamples, !sampleFormat.littleEndian);

        if (dataType == SpectrogramControls::RealComplexInterleaved) {
            for (int i = 0; i < samples/2; i++) {
                buffer[i][0] = double(sBuffer[i*2]) / weight;
                buffer[i][1] = double(sBuffer[i*2 + 1]) / weight;
            }
        }
        else {
            for (int i = 0; i < samples; i++) {
                buffer[i][0] = double(sBuffer[i]) / weight;
                buffer[i][1] = 0.0;
            }
        }
        delete[] sBuffer;
    }
    else if (sampleFormat.id.startsWith("uint32")) {
        quint32 *sBuffer = new quint32[maxSamples];
        samples = bitContainer->bits()->readUInt32Samples(sBuffer, offset, maxSamples, !sampleFormat.littleEndian);

        if (dataType == SpectrogramControls::RealComplexInterleaved) {
            for (int i = 0; i < samples/2; i++) {
                buffer[i][0] = double(sBuffer[i*2]) / weight;
                buffer[i][1] = double(sBuffer[i*2 + 1]) / weight;
            }
        }
        else {
            for (int i = 0; i < samples; i++) {
                buffer[i][0] = double(sBuffer[i]) / weight;
                buffer[i][1] = 0.0;
            }
        }
        delete[] sBuffer;
    }
    else if (sampleFormat.id.startsWith("int64")) {
        qint64 *sBuffer = new qint64[maxSamples];
        samples = bitContainer->bits()->readInt64Samples(sBuffer, offset, maxSamples, !sampleFormat.littleEndian);

        if (dataType == SpectrogramControls::RealComplexInterleaved) {
            for (int i = 0; i < samples/2; i++) {
                buffer[i][0] = double(sBuffer[i*2]) / weight;
                buffer[i][1] = double(sBuffer[i*2 + 1]) / weight;
            }
        }
        else {
            for (int i = 0; i < samples; i++) {
                buffer[i][0] = double(sBuffer[i]) / weight;
                buffer[i][1] = 0.0;
            }
        }
        delete[] sBuffer;
    }
    else if (sampleFormat.id.startsWith("uint64")) {
        quint64 *sBuffer = new quint64[maxSamples];
        samples = bitContainer->bits()->readUInt64Samples(sBuffer, offset, maxSamples, !sampleFormat.littleEndian);

        if (dataType == SpectrogramControls::RealComplexInterleaved) {
            for (int i = 0; i < samples/2; i++) {
                buffer[i][0] = double(sBuffer[i*2]) / weight;
                buffer[i][1] = double(sBuffer[i*2 + 1]) / weight;
            }
        }
        else {
            for (int i = 0; i < samples; i++) {
                buffer[i][0] = double(sBuffer[i]) / weight;
                buffer[i][1] = 0.0;
            }
        }
        delete[] sBuffer;
    }

    for (qint64 i = samples; i < fftSize; i++) {
        buffer[i][0] = 0.0;
        buffer[i][1] = 0.0;
    }
}

void Spectrogram::setSpectrums(QList<QVector<double> > spectrums)
{
    QMutexLocker lock(&m_mutex);
    m_spectrums = spectrums;
}
