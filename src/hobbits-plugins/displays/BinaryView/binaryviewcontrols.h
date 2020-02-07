#ifndef BINARYVIEWCONTROLS_H
#define BINARYVIEWCONTROLS_H

#include <QWidget>

namespace Ui
{
class BinaryViewControls;
}

class BinaryViewControls : public QWidget
{
    Q_OBJECT

public:
    BinaryViewControls();

signals:
    void fontSizeChanged(int);
    void columnGroupingChanged(int);
    void showHeadersChanged(bool);

private slots:
    void on_cb_showHeaders_stateChanged(int state);

private:
    Ui::BinaryViewControls *ui;
};

#endif // BINARYVIEWCONTROLS_H
