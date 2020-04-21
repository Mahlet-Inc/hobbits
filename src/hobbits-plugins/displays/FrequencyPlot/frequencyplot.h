#ifndef FREQUENCYPLOT_H
#define FREQUENCYPLOT_H

#include "displayinterface.h"
#include "frequencyplotcontrols.h"
#include "frequencyplotwidget.h"

class FrequencyPlot : public QObject, DisplayInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "hobbits.DisplayInterface.FrequencyPlot")
    Q_INTERFACES(DisplayInterface)

public:
    FrequencyPlot();

    DisplayInterface* createDefaultDisplay();

    QString getName();

    QWidget* getDisplayWidget(QSharedPointer<DisplayHandle> displayHandle);
    QWidget* getControlsWidget(QSharedPointer<DisplayHandle> displayHandle);

private:
    void initialize(QSharedPointer<DisplayHandle> displayHandle);
    FrequencyPlotWidget* m_displayWidget;
    FrequencyPlotControls* m_controlsWidget;
};

#endif // FREQUENCYPLOT_H
