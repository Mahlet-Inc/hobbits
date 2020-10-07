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

    DisplayInterface* createDefaultDisplay() override;

    QString name() override;
    QString description() override;
    QStringList tags() override;

    QWidget* display(QSharedPointer<DisplayHandle> displayHandle) override;
    QWidget* controls(QSharedPointer<DisplayHandle> displayHandle) override;

private:
    void initialize(QSharedPointer<DisplayHandle> displayHandle);
    FrequencyPlotWidget* m_displayWidget;
    FrequencyPlotControls* m_controlsWidget;
};

#endif // FREQUENCYPLOT_H
