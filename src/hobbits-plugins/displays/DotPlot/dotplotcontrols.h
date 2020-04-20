#ifndef DOTPLOTCONTROLS_H
#define DOTPLOTCONTROLS_H

#include <QWidget>

namespace Ui
{
class DotPlotControls;
}

class DotPlotControls : public QWidget
{
    Q_OBJECT

public:
    DotPlotControls();

signals:
    void newWindowSize(int);
    void newWordSize(int);
    void newScale(int);

private:
    Ui::DotPlotControls *ui;
};

#endif // DOTPLOTCONTROLS_H
