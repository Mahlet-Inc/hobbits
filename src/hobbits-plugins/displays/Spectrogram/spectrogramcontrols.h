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

public slots:
    void setSampleFormat(QString id);

signals:
    void overlapSet(int);
    void fftSizeSet(int);
    void sampleFormatSet(QString);
    void dataTypeSet(int);
    void sensitivitySet(double);
    void sampleRateSet(double);
    void headersShowSet(bool);
    void sliceShowSet(bool);
    void logarithmicSet(bool);

private slots:
    void on_cb_sampleFormat_currentIndexChanged(int index);
    void on_cb_dataType_currentIndexChanged(int index);
    void on_hs_sensitivity_valueChanged(int value);

    void on_sb_sampleRate_valueChanged(double arg1);

    void on_cb_rateUnits_currentIndexChanged(int index);

private:
    Ui::SpectrogramControls *ui;
};

#endif // SPECTROGRAMCONTROLS_H
