#ifndef BYTERASTERCONTROLS_H
#define BYTERASTERCONTROLS_H

#include <QWidget>

namespace Ui
{
class ByteRasterControls;
}

class ByteRasterControls : public QWidget
{
    Q_OBJECT

public:
    ByteRasterControls();

signals:
    void scaleSet(int);
    void showHeadersChanged(bool);

public slots:
    void on_cb_showHeaders_stateChanged(int state);

private:
    Ui::ByteRasterControls *ui;
};

#endif // BYTERASTERCONTROLS_H
