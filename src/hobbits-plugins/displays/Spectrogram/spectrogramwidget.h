#ifndef SPECTROGRAMWIDGET_H
#define SPECTROGRAMWIDGET_H

#include "displaybase.h"

class SpectrogramWidget : public DisplayBase
{
    Q_OBJECT

public:
    SpectrogramWidget(
            QSharedPointer<DisplayHandle> displayHandle,
            DisplayInterface *pluginRef,
            QWidget *parent = nullptr);

    void paintEvent(QPaintEvent*) override;
    void mouseMoveEvent(QMouseEvent *event) override;

public slots:
    void setScale(int);
    void setStride(int);
    void setFftSizeFactor(int);
    void setWordSize(int);

private:
    QList<QVector<double>> computeStft(int wordSize, int windowSize, int stride, int maxSpectrums, qint64 bitOffset, QSharedPointer<BitContainer> container);
    void prepareHeaders();

    int m_scale;
    bool m_showFrameOffsets;
    bool m_showColumnOffsets;

    int m_wordSize;
    int m_stride;
    int m_fftSizeFactor;

    QPoint m_displayOffset;
    QSize m_headerFontSize;

protected slots:
    void adjustScrollbars() override;
};

#endif // SPECTROGRAMWIDGET_H
