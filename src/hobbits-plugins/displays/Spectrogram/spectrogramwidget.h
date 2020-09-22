#ifndef SPECTROGRAMWIDGET_H
#define SPECTROGRAMWIDGET_H

#include "displaybase.h"
#include "spectrogramrenderer.h"
#include <QThread>
#include <QSemaphore>

class SpectrogramWidget : public DisplayBase
{
    Q_OBJECT

public:
    SpectrogramWidget(
            QSharedPointer<DisplayHandle> displayHandle,
            DisplayInterface *pluginRef,
            QWidget *parent = nullptr);

    void paintEvent(QPaintEvent*) override;
    void leaveEvent(QEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

public slots:
    void setScale(int);
    void setOverlap(int);
    void setFftSize(int);
    void setSampleFormat(QString);
    void setDataType(int);
    void setSensitivity(double);
    void setSampleRate(double);
    void setShowHeaders(bool);
    void setShowHoverSlices(bool);
    void setLogarithmic(bool);

signals:
    void sampleFormatChanged(QString);

private:
    void prepareHeaders();
    QString timeString(qint64 sample);
    QString getFreq(double percent);

    int m_scale;
    bool m_showFrameOffsets;
    bool m_showColumnOffsets;
    bool m_showHoverSlices;

    int m_hoverX;
    int m_hoverY;

    QPoint m_displayOffset;
    QSize m_displayCenterSize;
    QSize m_headerFontSize;

    QList<QVector<double>> m_spectrums;
    QImage m_spectrogram;

    SpectrogramRenderer * m_renderer;

    QSemaphore m_paintSemaphore;

protected slots:
    void adjustScrollbars() override;
};

#endif // SPECTROGRAMWIDGET_H
