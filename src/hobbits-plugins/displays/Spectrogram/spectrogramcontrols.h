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
    void overlapSet(int);
    void fftSizeSet(int);
    void wordFormatSet(int);
    void dataTypeSet(int);
    void sensitivitySet(double);

private slots:
    void on_cb_wordFormat_currentIndexChanged(int index);
    void on_cb_dataType_currentIndexChanged(int index);
    void on_hs_sensitivity_valueChanged(int value);

private:
    Ui::SpectrogramControls *ui;
};

#endif // SPECTROGRAMCONTROLS_H
