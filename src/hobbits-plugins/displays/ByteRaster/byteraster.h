#ifndef BYTERASTER_H
#define BYTERASTER_H

#include "byterastercontrols.h"
#include "byterasterwidget.h"
#include "displayinterface.h"


class ByteRaster : public QObject, DisplayInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "hobbits.DisplayInterface.ByteRaster")
    Q_INTERFACES(DisplayInterface)

public:
    ByteRaster();

    DisplayInterface* createDefaultDisplay();

    QString getName();

    QWidget* getDisplayWidget(QSharedPointer<DisplayHandle> displayHandle);
    QWidget* getControlsWidget(QSharedPointer<DisplayHandle> displayHandle);

private:
    void initialize(QSharedPointer<DisplayHandle> displayHandle);

    ByteRasterWidget *m_displayWidget;
    ByteRasterControls *m_controlsWidget;
};

#endif // BYTERASTER_H
