#ifndef HILBERTPLOTCONTROLS_H
#define HILBERTPLOTCONTROLS_H

#include <QWidget>

namespace Ui
{
class HilbertPlotControls;
}

class HilbertPlotControls : public QWidget
{
    Q_OBJECT

public:
    HilbertPlotControls();

private:
    Ui::HilbertPlotControls *ui;
};

#endif // HILBERTPLOTCONTROLS_H
