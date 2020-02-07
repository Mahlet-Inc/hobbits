#ifndef HEXVIEWCONTROLS_H
#define HEXVIEWCONTROLS_H

#include <QWidget>

namespace Ui
{
class HexViewControls;
}

class HexViewControls : public QWidget
{
    Q_OBJECT

public:
    HexViewControls();

signals:
    void fontSizeChanged(int);
    void columnGroupingChanged(int);
    void showHeadersChanged(bool);

private slots:
    void on_cb_showHeaders_stateChanged(int state);

private:
    Ui::HexViewControls *ui;
};

#endif // HEXVIEWCONTROLS_H
