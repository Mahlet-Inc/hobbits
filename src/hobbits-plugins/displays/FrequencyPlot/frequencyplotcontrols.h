#ifndef FREQUENCYPLOTCONTROLS_H
#define FREQUENCYPLOTCONTROLS_H

#include <QWidget>

namespace Ui
{
class FrequencyPlotControls;
}

class FrequencyPlotControls : public QWidget
{
    Q_OBJECT

public:
    FrequencyPlotControls();

signals:
    void newWindowSize(int);
    void newWordSize(int);
    void newScale(int);

private:
    Ui::FrequencyPlotControls *ui;
};

#endif // FREQUENCYPLOTCONTROLS_H
