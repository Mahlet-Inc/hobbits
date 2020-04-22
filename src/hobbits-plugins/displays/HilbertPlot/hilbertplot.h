#ifndef HILBERTPLOT_H
#define HILBERTPLOT_H

#include "displayinterface.h"
#include "hilbertplotcontrols.h"
#include "hilbertplotwidget.h"

class HilbertPlot : public QObject, DisplayInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "hobbits.DisplayInterface.HilbertPlot")
    Q_INTERFACES(DisplayInterface)

public:
    HilbertPlot();

    DisplayInterface* createDefaultDisplay();

    QString getName();

    QWidget* getDisplayWidget(QSharedPointer<DisplayHandle> displayHandle);
    QWidget* getControlsWidget(QSharedPointer<DisplayHandle> displayHandle);

private:
    void initialize(QSharedPointer<DisplayHandle> displayHandle);
    HilbertPlotWidget* m_displayWidget;
    HilbertPlotControls* m_controlsWidget;
};

#endif // HILBERTPLOT_H
