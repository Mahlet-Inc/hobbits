#include "spectrogramrenderer.h"
#include <QtMath>
#include <QImage>
#include "settingsmanager.h"
#include <QtConcurrent/QtConcurrentRun>
#include <QTime>
#include <QTimer>
#include "viridis.h"
#include "metadatahelper.h"

SpectrogramRenderer::SpectrogramRenderer(QObject *parent) :
    QObject(parent),
    m_maxSpectrums(1),
    m_bitOffset(0),
    m_overlap(4),
    m_fftSize(1024),
    m_dataType(DataType::Real),
    m_sensitivity(1.0),
    m_sampleRate(16000),
    m_logarithmic(true),
    m_renderDirty(true),
    m_rendering(false)
{
    m_sampleFormat = MetadataHelper::sampleFormats().at(0);
}

SpectrogramRenderer::~SpectrogramRenderer()
{
    if (m_watcher.isRunning()) {
        m_watcher.cancel();
    }
    m_watcher.waitForFinished();
}

qint64 SpectrogramRenderer::bitOffset() const
{
    return m_bitOffset;
}

void SpectrogramRenderer::setBitOffset(const qint64 &bitOffset)
{
    if (m_bitOffset != bitOffset) {
        m_mutex.lock();
        m_bitOffset = bitOffset;
        m_mutex.unlock();
        setDirty();
    }
}

int SpectrogramRenderer::wordSize() const
{
    return m_sampleFormat.wordSize;
}

int SpectrogramRenderer::overlap() const
{
    return m_overlap;
}

void SpectrogramRenderer::setOverlap(int overlap)
{
    if (m_overlap != overlap) {
        m_mutex.lock();
        m_overlap = overlap;
        m_mutex.unlock();
        setDirty();
    }
}

int SpectrogramRenderer::fftSize() const
{
    return m_fftSize;
}

void SpectrogramRenderer::setFftSize(int fftSize)
{
    if (m_fftSize != fftSize) {
        m_mutex.lock();
        m_fftSize = fftSize;
        m_mutex.unlock();
        setDirty();
    }
}

SpectrogramRenderer::DataType SpectrogramRenderer::dataType() const
{
    return m_dataType;
}

void SpectrogramRenderer::setDataType(const DataType &dataType)
{
    if (m_dataType != dataType) {
        m_mutex.lock();
        m_dataType = dataType;
        m_mutex.unlock();
        setDirty();
    }
}

double SpectrogramRenderer::sensitivity() const
{
    return m_sensitivity;
}

void SpectrogramRenderer::setSensitivity(double sensitivity)
{
    if (m_sensitivity != sensitivity) {
        m_mutex.lock();
        m_sensitivity = sensitivity;
        m_mutex.unlock();
        setDirty();
    }
}

double SpectrogramRenderer::sampleRate() const
{
    return m_sampleRate;
}

void SpectrogramRenderer::setSampleRate(double sampleRate)
{
    if (m_sampleRate != sampleRate) {
        m_mutex.lock();
        m_sampleRate = sampleRate;
        m_mutex.unlock();
        setDirty();
    }
}

void SpectrogramRenderer::setContainer(const QSharedPointer<BitContainer> &container)
{
    if (m_container != container) {
        m_mutex.lock();
        m_container = container;
        QVariant sampleFormat = m_container->info()->metadata(MetadataHelper::sampleFormatKey());
        if (sampleFormat.isValid()) {
            m_sampleFormat = MetadataHelper::sampleFormat(sampleFormat.toString());
        }
        else {
            m_container->info()->setMetadata(MetadataHelper::sampleFormatKey(), m_sampleFormat.id);
        }
        m_mutex.unlock();
        setDirty();
    }
}

QSharedPointer<BitContainer> SpectrogramRenderer::container() const
{
    return m_container;
}

int SpectrogramRenderer::maxSpectrums() const
{
    return m_maxSpectrums;
}

void SpectrogramRenderer::setMaxSpectrums(int maxSpectrums)
{
    if (m_maxSpectrums != maxSpectrums) {
        m_mutex.lock();
        m_maxSpectrums = maxSpectrums;
        m_mutex.unlock();
        setDirty();
    }
}

void SpectrogramRenderer::setDirty()
{
    bool locked = m_mutex.tryLock();
    m_renderDirty = true;
    if (!m_rendering) {
        if (m_watcher.isRunning()) {
            m_watcher.cancel();
            if (locked) {
                m_mutex.unlock();
                locked = false;
            }
            m_watcher.waitForFinished();
            locked = m_mutex.tryLock();
        }
        auto future = QtConcurrent::run(&SpectrogramRenderer::computeStft, this);
        m_watcher.setFuture(future);
    }
    if (locked) {
        m_mutex.unlock();
    }
}

int SpectrogramRenderer::bitStride() const
{
    int strideBits = m_fftSize;
    if (m_overlap > 0) {
        strideBits = m_fftSize / m_overlap;
    }
    strideBits *= m_sampleFormat.wordSize;
    return strideBits;
}

MetadataHelper::SampleFormat SpectrogramRenderer::sampleFormat() const
{
    return m_sampleFormat;
}

void SpectrogramRenderer::setSampleFormat(const QString &sampleFormat)
{
    if (m_sampleFormat.id != sampleFormat) {
        m_mutex.lock();
        m_sampleFormat = MetadataHelper::sampleFormat(sampleFormat);
        m_mutex.unlock();
        if (!m_container.isNull()) {
            m_container->info()->setMetadata(MetadataHelper::sampleFormatKey(), m_sampleFormat.id);
        }
        setDirty();
    }
}

void SpectrogramRenderer::fillSamples(fftw_complex* buffer, qint64 offset, SpectrogramRenderer *renderer)
{
    offset /= renderer->sampleFormat().wordSize;
    qint64 maxSamples = renderer->fftSize();
    if (renderer->dataType() == RealComplexInterleaved) {
        maxSamples *= 2;
    }
    qint64 samples = 0;
    double weight = double(1ull << (renderer->sampleFormat().wordSize - 1));
    if (renderer->sampleFormat().id.startsWith("float32")) {
        float *fBuffer = new float[maxSamples];
        samples = renderer->container()->bits()->readFloat32Samples(fBuffer, offset, maxSamples, !renderer->sampleFormat().littleEndian);

        if (renderer->dataType() == RealComplexInterleaved) {
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
    else if (renderer->sampleFormat().id.startsWith("float64")) {
        double *sBuffer = new double[maxSamples];
        samples = renderer->container()->bits()->readFloat64Samples(sBuffer, offset, maxSamples, !renderer->sampleFormat().littleEndian);

        if (renderer->dataType() == RealComplexInterleaved) {
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
    else if (renderer->sampleFormat().id.startsWith("int8")) {
        qint8 *sBuffer = new qint8[maxSamples];
        samples = renderer->container()->bits()->readBytes(reinterpret_cast<char*>(sBuffer), offset, maxSamples);

        if (renderer->dataType() == RealComplexInterleaved) {
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
    else if (renderer->sampleFormat().id.startsWith("uint8")) {
        quint8 *sBuffer = new quint8[maxSamples];
        samples = renderer->container()->bits()->readBytes(reinterpret_cast<char*>(sBuffer), offset, maxSamples);

        if (renderer->dataType() == RealComplexInterleaved) {
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
    else if (renderer->sampleFormat().id.startsWith("int16")) {
        qint16 *sBuffer = new qint16[maxSamples];
        samples = renderer->container()->bits()->readInt16Samples(sBuffer, offset, maxSamples, !renderer->sampleFormat().littleEndian);

        if (renderer->dataType() == RealComplexInterleaved) {
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
    else if (renderer->sampleFormat().id.startsWith("uint16")) {
        quint16 *sBuffer = new quint16[maxSamples];
        samples = renderer->container()->bits()->readUInt16Samples(sBuffer, offset, maxSamples, !renderer->sampleFormat().littleEndian);

        if (renderer->dataType() == RealComplexInterleaved) {
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
    else if (renderer->sampleFormat().id.startsWith("int24")) {
        qint32 *sBuffer = new qint32[maxSamples];
        samples = renderer->container()->bits()->readInt24Samples(sBuffer, offset, maxSamples, !renderer->sampleFormat().littleEndian);

        if (renderer->dataType() == RealComplexInterleaved) {
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
    else if (renderer->sampleFormat().id.startsWith("uint24")) {
        quint32 *sBuffer = new quint32[maxSamples];
        samples = renderer->container()->bits()->readUInt24Samples(sBuffer, offset, maxSamples, !renderer->sampleFormat().littleEndian);

        if (renderer->dataType() == RealComplexInterleaved) {
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
    else if (renderer->sampleFormat().id.startsWith("int32")) {
        qint32 *sBuffer = new qint32[maxSamples];
        samples = renderer->container()->bits()->readInt32Samples(sBuffer, offset, maxSamples, !renderer->sampleFormat().littleEndian);

        if (renderer->dataType() == RealComplexInterleaved) {
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
    else if (renderer->sampleFormat().id.startsWith("uint32")) {
        quint32 *sBuffer = new quint32[maxSamples];
        samples = renderer->container()->bits()->readUInt32Samples(sBuffer, offset, maxSamples, !renderer->sampleFormat().littleEndian);

        if (renderer->dataType() == RealComplexInterleaved) {
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
    else if (renderer->sampleFormat().id.startsWith("int64")) {
        qint64 *sBuffer = new qint64[maxSamples];
        samples = renderer->container()->bits()->readInt64Samples(sBuffer, offset, maxSamples, !renderer->sampleFormat().littleEndian);

        if (renderer->dataType() == RealComplexInterleaved) {
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
    else if (renderer->sampleFormat().id.startsWith("uint64")) {
        quint64 *sBuffer = new quint64[maxSamples];
        samples = renderer->container()->bits()->readUInt64Samples(sBuffer, offset, maxSamples, !renderer->sampleFormat().littleEndian);

        if (renderer->dataType() == RealComplexInterleaved) {
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

    for (qint64 i = samples; i < renderer->fftSize(); i++) {
        buffer[i][0] = 0.0;
        buffer[i][1] = 0.0;
    }
}

void SpectrogramRenderer::computeStft(SpectrogramRenderer *renderer)
{
    renderer->m_mutex.lock();
    renderer->m_renderDirty = false;
    renderer->m_rendering = true;

    if (renderer->container().isNull()) {
        renderer->m_rendering = false;
        renderer->m_mutex.unlock();
        return;
    }

    QList<QVector<double>> spectrums;
    QImage img(renderer->fftSize()/2, renderer->maxSpectrums(), QImage::Format_ARGB32);
    img.fill(Qt::lightGray);
    QMetaObject::invokeMethod(renderer, "sendSpectrums", Qt::QueuedConnection, Q_ARG(const QList<QVector<double> > &, spectrums), Q_ARG(const QImage &, img));

    if (renderer->wordSize() > 64 || renderer->wordSize() < 1) {
        return;
    }

    fftw_complex *fftIn = reinterpret_cast<fftw_complex*>(fftw_malloc(sizeof(fftw_complex) * unsigned(renderer->fftSize())));
    fftw_complex *fftOut = reinterpret_cast<fftw_complex*>(fftw_malloc(sizeof(fftw_complex) * unsigned(renderer->fftSize())));
    fftw_plan plan = fftw_plan_dft_1d(renderer->fftSize(), fftIn, fftOut, FFTW_FORWARD, FFTW_ESTIMATE);
    renderer->m_mutex.unlock();

    QVector<double> hanningWindow(renderer->fftSize());
    for (int i = 0; i < renderer->fftSize(); i++) {
        hanningWindow[i] = 0.5*(1.0-cos(2.0*M_PI*i/double(renderer->fftSize()-1.0)));
    }

    double outputFactor = 1.0 / double(renderer->fftSize());
    if (renderer->logarithmic()) {
        outputFactor *= (renderer->sensitivity() * renderer->sensitivity());
    }
    else {
        outputFactor *= renderer->sensitivity();
    }

    int fftBits = renderer->fftSize()*renderer->wordSize();
    int strideBits = renderer->bitStride();

    qint64 currOffset = renderer->bitOffset();
    currOffset /= fftBits;
    currOffset *= fftBits;

    QTime lastTime = QTime::currentTime();
    for (int i = 0; i < renderer->maxSpectrums(); i++) {
        renderer->m_mutex.lock();
        if (renderer->m_watcher.isCanceled()) {
            renderer->m_rendering = false;
            renderer->m_mutex.unlock();
            return;
        }
        if (renderer->m_renderDirty) {
            renderer->m_rendering = false;
            fftw_destroy_plan(plan);
            fftw_free(fftIn);
            fftw_free(fftOut);
            renderer->m_mutex.unlock();
            QTimer::singleShot(10, renderer, [renderer]() {
                renderer->setDirty();
            });
            return;
        }

        if (currOffset + fftBits >= renderer->container()->bits()->sizeInBits()) {
            renderer->m_mutex.unlock();
            break;
        }
        fillSamples(fftIn, currOffset, renderer);

        for (int i = 0; i < renderer->fftSize(); i++) {
            fftIn[i][0] *= hanningWindow[i];
            fftIn[i][1] *= hanningWindow[i];
        }
        fftw_execute_dft(plan, fftIn, fftOut);

        QVector<double> spectrum(renderer->fftSize()/2);
        if (renderer->logarithmic()) {
            for (int n = 0; n < renderer->fftSize()/2; n++) {
                spectrum[n] = log(outputFactor * ((fftOut[n][0] * fftOut[n][0]) + (fftOut[n][1] * fftOut[n][1]))) / log(10);
            }
        }
        else {
            for (int n = 0; n < renderer->fftSize()/2; n++) {
                spectrum[n] = outputFactor * ((fftOut[n][0] * fftOut[n][0]) + (fftOut[n][1] * fftOut[n][1]));
            }
        }
        spectrums.append(spectrum);
        for (int x = 0; x < img.width()  && x < spectrum.size(); x++) {
            img.setPixel(x, i, VIRIDIS_MAP[qBound(0, qFloor(spectrum.at(x) * 256.0), 255)].rgb());
        }

        renderer->m_mutex.unlock();

        currOffset += strideBits;

        if (lastTime.msecsTo(QTime::currentTime()) > 400) {
            QMetaObject::invokeMethod(renderer, "sendSpectrums", Qt::QueuedConnection, Q_ARG(const QList<QVector<double> > &, spectrums), Q_ARG(const QImage &, img));
            lastTime = QTime::currentTime();
        }
    }
    QMetaObject::invokeMethod(renderer, "sendSpectrums", Qt::QueuedConnection, Q_ARG(const QList<QVector<double> > &, spectrums), Q_ARG(const QImage &, img));

    renderer->m_mutex.lock();
    fftw_destroy_plan(plan);
    fftw_free(fftIn);
    fftw_free(fftOut);
    renderer->m_mutex.unlock();

    if (renderer->m_renderDirty) {
        computeStft(renderer);
    }
    else {
        renderer->m_mutex.lock();
        renderer->m_rendering = false;
        renderer->m_mutex.unlock();
    }
}

bool SpectrogramRenderer::logarithmic() const
{
    return m_logarithmic;
}

void SpectrogramRenderer::setLogarithmic(bool logarithmic)
{
    if (!m_logarithmic == logarithmic) {
        m_mutex.lock();
        m_logarithmic = logarithmic;
        m_mutex.unlock();
        setDirty();
    }
}

void SpectrogramRenderer::sendSpectrums(const QList<QVector<double> > &s, const QImage &img)
{
    emit spectrumsChanged(s, img);
}

