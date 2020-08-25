#ifndef SPECTROGRAMWIDGET_H
#define SPECTROGRAMWIDGET_H

#include "displaybase.h"
#include "fftw3.h"

class SpectrogramWidget : public DisplayBase
{
    Q_OBJECT

public:
    enum WordFormat {
        Unsigned = 1,
        TwosComplement = 2
    };

    enum DataType {
        Real = 1,
        RealComplexInterleaved = 2
    };

    SpectrogramWidget(
            QSharedPointer<DisplayHandle> displayHandle,
            DisplayInterface *pluginRef,
            QWidget *parent = nullptr);

    void paintEvent(QPaintEvent*) override;
    void mouseMoveEvent(QMouseEvent *event) override;

public slots:
    void setScale(int);
    void setOverlap(int);
    void setFftSize(int);
    void setWordSize(int);
    void setWordFormat(int);
    void setDataType(int);
    void setSensitivity(double);

private:
    int bitStride();
    void fillSamples(fftw_complex* buffer, int sampleCount, qint64 bitOffset, QSharedPointer<BitContainer> container);
    QList<QVector<double>> computeStft(int maxSpectrums, qint64 bitOffset, QSharedPointer<BitContainer> container);
    void prepareHeaders();

    int m_scale;
    bool m_showFrameOffsets;
    bool m_showColumnOffsets;

    int m_wordSize;
    int m_overlap;
    int m_fftSize;
    WordFormat m_wordFormat;
    DataType m_dataType;

    double m_sensitivity;

    QPoint m_displayOffset;
    QSize m_headerFontSize;

protected slots:
    void adjustScrollbars() override;
};

#endif // SPECTROGRAMWIDGET_H
