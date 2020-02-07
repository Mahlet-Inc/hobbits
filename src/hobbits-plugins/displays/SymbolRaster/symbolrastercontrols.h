#ifndef SYMBOLRASTERCONTROLS_H
#define SYMBOLRASTERCONTROLS_H

#include "colormapmodel.h"
#include <QWidget>

namespace Ui
{
class SymbolRasterControls;
}

class SymbolRasterControls : public QWidget
{
    Q_OBJECT

public:
    SymbolRasterControls();

public slots:
    void on_cb_showHeaders_stateChanged(int state);
    void updateColorMap();

signals:
    void scaleSet(int);
    void showHeadersChanged(bool);
    void colorMappingChanged(QList<QPair<QString, QColor>>);

private:
    Ui::SymbolRasterControls *ui;
    ColorMapModel *m_colorMapModel;
};

#endif // SYMBOLRASTERCONTROLS_H
