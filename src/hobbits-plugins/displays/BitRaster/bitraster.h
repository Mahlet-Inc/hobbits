#ifndef BITRASTER_H
#define BITRASTER_H

#include "bitrastercontrols.h"
#include "bitrasterwidget.h"
#include "displayinterface.h"


class BitRaster : public QObject, DisplayInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "hobbits.DisplayInterface.BitRaster")
    Q_INTERFACES(DisplayInterface)

public:
    BitRaster();

    DisplayInterface* createDefaultDisplay();

    QString getName();

    QWidget* getDisplayWidget(QSharedPointer<DisplayHandle> displayHandle);
    QWidget* getControlsWidget(QSharedPointer<DisplayHandle> displayHandle);

private:
    void initialize(QSharedPointer<DisplayHandle> displayHandle);

    BitRasterWidget *m_displayWidget;
    BitRasterControls *m_controlsWidget;
};

#endif // BITRASTER_H
