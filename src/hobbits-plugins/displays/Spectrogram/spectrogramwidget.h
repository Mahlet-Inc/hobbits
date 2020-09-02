#ifndef SPECTROGRAMWIDGET_H
#define SPECTROGRAMWIDGET_H

#include "displaybase.h"
#include "spectrogramrenderer.h"
#include <QThread>

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
    void setOverlap(int);
    void setFftSize(int);
    void setWordSize(int);
    void setWordFormat(int);
    void setDataType(int);
    void setSensitivity(double);
    void setSampleRate(double);
    void setShowHeaders(bool);

private:
    void prepareHeaders();
    QString timeString(qint64 sample);

    int m_scale;
    bool m_showFrameOffsets;
    bool m_showColumnOffsets;

    QPoint m_displayOffset;
    QSize m_headerFontSize;

    QList<QVector<double>> m_spectrums;
    QImage m_spectrogram;

    SpectrogramRenderer * m_renderer;

protected slots:
    void adjustScrollbars() override;
};

#endif // SPECTROGRAMWIDGET_H
