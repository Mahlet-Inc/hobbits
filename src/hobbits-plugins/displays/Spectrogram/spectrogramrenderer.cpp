#include "spectrogramrenderer.h"
#include <QtMath>
#include <QImage>
#include "settingsmanager.h"
#include <QtConcurrent/QtConcurrentRun>
#include <QTime>
#include <QTimer>
#include "viridis.h"

#ifdef Q_OS_UNIX
#define bswap32(X) __builtin_bswap32((X))
#define bswap64(X) __builtin_bswap64((X))
#endif
#ifdef Q_OS_WIN
#include <intrin.h>
#define bswap32(X) _byteswap_ulong((X))
#define bswap64(X) _byteswap_uint64((X))
#endif

SpectrogramRenderer::SpectrogramRenderer(QObject *parent) :
    QObject(parent),
    m_maxSpectrums(1),
    m_bitOffset(0),
    m_wordSize(8),
    m_overlap(4),
    m_fftSize(1024),
    m_wordFormat(WordFormat::Unsigned),
    m_dataType(DataType::Real),
    m_sensitivity(1.0),
    m_sampleRate(16000),
    m_logarithmic(true),
    m_renderDirty(true),
    m_rendering(false)
{

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
        QMutexLocker lock(&m_mutex);
        m_bitOffset = bitOffset;
        setDirty();
    }
}

int SpectrogramRenderer::wordSize() const
{
    return m_wordSize;
}

void SpectrogramRenderer::setWordSize(int wordSize)
{
    if (m_wordSize != wordSize) {
        QMutexLocker lock(&m_mutex);
        m_wordSize = wordSize;
        setDirty();
    }
}

int SpectrogramRenderer::overlap() const
{
    return m_overlap;
}

void SpectrogramRenderer::setOverlap(int overlap)
{
    if (m_overlap != overlap) {
        QMutexLocker lock(&m_mutex);
        m_overlap = overlap;
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
        QMutexLocker lock(&m_mutex);
        m_fftSize = fftSize;
        setDirty();
    }
}

int SpectrogramRenderer::wordFormat() const
{
    return m_wordFormat;
}

void SpectrogramRenderer::setWordFormat(int wordFormat)
{
    if (m_wordFormat != wordFormat) {
        QMutexLocker lock(&m_mutex);
        m_wordFormat = wordFormat;
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
        QMutexLocker lock(&m_mutex);
        m_dataType = dataType;
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
        QMutexLocker lock(&m_mutex);
        m_sensitivity = sensitivity;
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
        QMutexLocker lock(&m_mutex);
        m_sampleRate = sampleRate;
        setDirty();
    }
}

void SpectrogramRenderer::setContainer(const QSharedPointer<BitContainer> &container)
{
    if (m_container != container) {
        QMutexLocker lock(&m_mutex);
        m_container = container;
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
        QMutexLocker lock(&m_mutex);
        m_maxSpectrums = maxSpectrums;
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
            m_watcher.waitForFinished();
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
    strideBits *= m_wordSize;
    return strideBits;
}

void SpectrogramRenderer::fillSamples(fftw_complex* buffer, qint64 offset, SpectrogramRenderer *renderer)
{
    if (renderer->wordFormat() & IEEE_754) {
        if (renderer->wordSize() == 32) {
            qint64 maxBytes = renderer->fftSize() * 4;
            if (renderer->dataType() == RealComplexInterleaved) {
                maxBytes *= 2;
            }
            float *fBuffer = new float[maxBytes / 4];
            qint64 samples = renderer->container()->bits()->readBytes(reinterpret_cast<char*>(fBuffer), offset / 8, maxBytes);
            samples /= 4;

            if (!(renderer->wordFormat() & LittleEndian)) {
                quint32* endianBuff = reinterpret_cast<quint32*>(fBuffer);
                for (int i = 0; i < samples; i++) {
                    endianBuff[i] = bswap32(endianBuff[i]);
                }
            }

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
            for (qint64 i = samples; i < renderer->fftSize(); i++) {
                buffer[i][0] = 0.0;
                buffer[i][1] = 0.0;
            }
            delete[] fBuffer;
        }
        else if (renderer->wordSize() == 64) {
            qint64 maxBytes = renderer->fftSize() * 8;
            if (renderer->dataType() == RealComplexInterleaved) {
                maxBytes *= 2;
            }
            double *dBuffer = new double[maxBytes / 8];
            qint64 samples = renderer->container()->bits()->readBytes(reinterpret_cast<char*>(dBuffer), offset / 8, maxBytes);
            samples /= 8;

            if (!(renderer->wordFormat() & LittleEndian)) {
                quint64* endianBuff = reinterpret_cast<quint64*>(dBuffer);
                for (int i = 0; i < samples; i++) {
                    endianBuff[i] = bswap64(endianBuff[i]);
                }
            }

            if (renderer->dataType() == RealComplexInterleaved) {
                for (int i = 0; i < samples/2; i++) {
                    buffer[i][0] = dBuffer[i*2];
                    buffer[i][1] = dBuffer[i*2 + 1];
                }
            }
            else {
                for (int i = 0; i < samples; i++) {
                    buffer[i][0] = dBuffer[i];
                    buffer[i][1] = 0.0;
                }
            }
            for (qint64 i = samples; i < renderer->fftSize(); i++) {
                buffer[i][0] = 0.0;
                buffer[i][1] = 0.0;
            }
            delete[] dBuffer;
        }
        else {
            for (int i = 0; i < renderer->fftSize(); i++) {
                buffer[i][0] = 0.0;
                buffer[i][1] = 0.0;
            }
        }
        return;
    }


    quint64 maxWordValue = 1;
    maxWordValue <<= (renderer->wordSize() - 1);
    double wordInverse = 1.0 / double(maxWordValue);
    int sampleSize = renderer->wordSize();
    bool withComplex = (renderer->dataType() == RealComplexInterleaved);
    if (withComplex) {
        sampleSize *= 2;
    }
    bool littleEndian = (renderer->wordFormat() & LittleEndian);
    for (int i = 0; i < renderer->fftSize(); i++) {
        if (offset+sampleSize >= renderer->container()->bits()->sizeInBits()) {
            buffer[i][0] = 0.0;
            buffer[i][1] = 0.0;
            continue;
        }
        if (renderer->wordFormat() & TwosComplement) {
            buffer[i][0] = double(renderer->container()->bits()->parseIntValue(offset, renderer->wordSize(), littleEndian)) * wordInverse;
        }
        else {
            buffer[i][0] = double(renderer->container()->bits()->parseUIntValue(offset, renderer->wordSize(), littleEndian)) * wordInverse;
        }
        if (withComplex) {
            if (renderer->wordFormat() & TwosComplement) {
                buffer[i][1] = double(renderer->container()->bits()->parseIntValue(offset + renderer->wordSize(), renderer->wordSize(), littleEndian)) * wordInverse;
            }
            else {
                buffer[i][1] = double(renderer->container()->bits()->parseUIntValue(offset + renderer->wordSize(), renderer->wordSize(), littleEndian)) * wordInverse;
            }
        }
        else {
            buffer[i][1] = 0.0;
        }
        offset += sampleSize;
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

    double outputFactor = 2.0 / double(renderer->fftSize());
    if (renderer->logarithmic()) {
        outputFactor *= (renderer->sensitivity() * renderer->sensitivity());
    }
    else {
        outputFactor *= renderer->sensitivity();
    }

    int fftBits = renderer->fftSize()*renderer->wordSize();
    int strideBits = renderer->bitStride();

    qint64 currOffset = renderer->bitOffset();

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
                spectrum[n] = log((fftOut[n][0] * fftOut[n][0] * outputFactor) + (fftOut[n][1] * fftOut[n][1] * outputFactor)) / log(10);
            }
        }
        else {
            for (int n = 0; n < renderer->fftSize()/2; n++) {
                spectrum[n] = (fftOut[n][0] * fftOut[n][0] * outputFactor) + (fftOut[n][1] * fftOut[n][1] * outputFactor);
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
        QMutexLocker lock(&m_mutex);
        m_logarithmic = logarithmic;
        setDirty();
    }
}

void SpectrogramRenderer::sendSpectrums(const QList<QVector<double> > &s, const QImage &img)
{
    emit spectrumsChanged(s, img);
}

