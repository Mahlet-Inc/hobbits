#ifndef SPECTROGRAMRENDERER_H
#define SPECTROGRAMRENDERER_H

#include <QObject>
#include <QMutex>
#include <QFutureWatcher>
#include "fftw3.h"
#include "bitcontainer.h"

class SpectrogramRenderer : public QObject
{
    Q_OBJECT
public:
    enum WordFormat {
        Unsigned = 0x00,
        TwosComplement = 0x01,
        BigEndian = 0x00,
        LittleEndian = 0x10,
        IEEE_754 = 0x80
    };

    enum DataType {
        Real = 1,
        RealComplexInterleaved = 2
    };

    explicit SpectrogramRenderer(QObject *parent = nullptr);
    ~SpectrogramRenderer();
    int bitStride() const;

    qint64 bitOffset() const;
    void setBitOffset(const qint64 &bitOffset);

    int wordSize() const;
    void setWordSize(int wordSize);

    int overlap() const;
    void setOverlap(int overlap);

    int fftSize() const;
    void setFftSize(int fftSize);

    int wordFormat() const;
    void setWordFormat(int wordFormat);

    DataType dataType() const;
    void setDataType(const DataType &dataType);

    double sensitivity() const;
    void setSensitivity(double sensitivity);

    double sampleRate() const;
    void setSampleRate(double sampleRate);

    void setContainer(const QSharedPointer<BitContainer> &container);

    QSharedPointer<BitContainer> container() const;

    int maxSpectrums() const;
    void setMaxSpectrums(int maxSpectrums);

    void setDirty();

signals:
    void spectrumsChanged(const QList<QVector<double>>&, const QImage&);

public slots:

private slots:
    void sendSpectrums(const QList<QVector<double>>&, const QImage&);

private:
    static void fillSamples(fftw_complex* buffer, qint64 offset, SpectrogramRenderer *renderer);
    static void computeStft(SpectrogramRenderer *renderer);
    static bool renderCheck(SpectrogramRenderer *renderer);

    int m_maxSpectrums;
    qint64 m_bitOffset;
    int m_wordSize;
    int m_overlap;
    int m_fftSize;
    int m_wordFormat;
    DataType m_dataType;
    double m_sensitivity;
    double m_sampleRate;
    QSharedPointer<BitContainer> m_container;

    QFutureWatcher<void> m_watcher;

    bool m_renderDirty;
    bool m_rendering;
    QMutex m_mutex;
};

#endif // SPECTROGRAMRENDERER_H
