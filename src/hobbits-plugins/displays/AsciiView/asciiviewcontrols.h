#ifndef ASCIIVIEWCONTROLS_H
#define ASCIIVIEWCONTROLS_H

#include <QWidget>

namespace Ui
{
class AsciiViewControls;
}

class AsciiViewControls : public QWidget
{
    Q_OBJECT

public:
    AsciiViewControls();

signals:
    void fontSizeChanged(int);
    void columnGroupingChanged(int);
    void showHeadersChanged(bool);

private slots:
    void on_cb_showHeaders_stateChanged(int state);

private:
    Ui::AsciiViewControls *ui;
};

#endif // ASCIIVIEWCONTROLS_H
