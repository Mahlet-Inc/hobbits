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

private:
    Ui::FrequencyPlotControls *ui;
};

#endif // FREQUENCYPLOTCONTROLS_H
