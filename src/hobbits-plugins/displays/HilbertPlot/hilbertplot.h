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

    DisplayInterface* createDefaultDisplay() override;

    QString name() override;
    QString description() override;
    QStringList tags() override;

    QWidget* display(QSharedPointer<DisplayHandle> displayHandle) override;
    QWidget* controls(QSharedPointer<DisplayHandle> displayHandle) override;

private:
    void initialize(QSharedPointer<DisplayHandle> displayHandle);
    HilbertPlotWidget* m_displayWidget;
    HilbertPlotControls* m_controlsWidget;
};

#endif // HILBERTPLOT_H
