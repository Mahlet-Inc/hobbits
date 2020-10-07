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

    DisplayInterface* createDefaultDisplay() override;

    QString name() override;
    QString description() override;
    QStringList tags() override;

    QWidget* display(QSharedPointer<DisplayHandle> displayHandle) override;
    QWidget* controls(QSharedPointer<DisplayHandle> displayHandle) override;

private:
    void initialize(QSharedPointer<DisplayHandle> displayHandle);
    DigraphPlotWidget* m_displayWidget;
    DigraphPlotControls* m_controlsWidget;
};

#endif // DIGRAPHPLOT_H
