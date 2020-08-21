#ifndef SPECTROGRAMCONTROLS_H
#define SPECTROGRAMCONTROLS_H

#include <QWidget>

namespace Ui
{
class SpectrogramControls;
}

class SpectrogramControls : public QWidget
{
    Q_OBJECT

public:
    SpectrogramControls();
    void sendCurrentValues();

signals:
    void wordSizeSet(int);
    void strideSet(int);
    void fftFactorSet(int);

private slots:
    void on_sb_stride_valueChanged(int val);
    void on_sb_fftFactor_valueChanged(int val);

    void calculateFftSize();

private:
    Ui::SpectrogramControls *ui;
};

#endif // SPECTROGRAMCONTROLS_H
