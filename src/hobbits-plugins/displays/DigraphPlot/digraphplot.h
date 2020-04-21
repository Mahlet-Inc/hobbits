#ifndef DIGRAPHPLOT_H
#define DIGRAPHPLOT_H

#include "displayinterface.h"
#include "digraphplotcontrols.h"
#include "digraphplotwidget.h"

class DigraphPlot : public QObject, DisplayInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "hobbits.DisplayInterface.DigraphPlot")
    Q_INTERFACES(DisplayInterface)

public:
    DigraphPlot();

    DisplayInterface* createDefaultDisplay();

    QString getName();

    QWidget* getDisplayWidget(QSharedPointer<DisplayHandle> displayHandle);
    QWidget* getControlsWidget(QSharedPointer<DisplayHandle> displayHandle);

private:
    void initialize(QSharedPointer<DisplayHandle> displayHandle);
    DigraphPlotWidget* m_displayWidget;
    DigraphPlotControls* m_controlsWidget;
};

#endif // DIGRAPHPLOT_H
