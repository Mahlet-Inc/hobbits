#ifndef BITRASTERCONTROLS_H
#define BITRASTERCONTROLS_H

#include <QWidget>

namespace Ui
{
class BitRasterControls;
}

class BitRasterControls : public QWidget
{
    Q_OBJECT

public:
    BitRasterControls();

signals:
    void scaleSet(int);
    void showHeadersChanged(bool);

public slots:
    void on_cb_showHeaders_stateChanged(int state);

private:
    Ui::BitRasterControls *ui;
};

#endif // BITRASTERCONTROLS_H
