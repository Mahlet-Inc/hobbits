#ifndef DIGRAPHPLOTCONTROLS_H
#define DIGRAPHPLOTCONTROLS_H

#include <QWidget>

namespace Ui
{
class DigraphPlotControls;
}

class DigraphPlotControls : public QWidget
{
    Q_OBJECT

public:
    DigraphPlotControls();

signals:
    void newWindowSize(int);
    void newWordSize(int);
    void newScale(int);

private:
    Ui::DigraphPlotControls *ui;
};

#endif // DIGRAPHPLOTCONTROLS_H
