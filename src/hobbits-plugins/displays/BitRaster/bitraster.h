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

    DisplayInterface* createDefaultDisplay() override;

    QString name() override;
    QString description() override;
    QStringList tags() override;

    QWidget* display(QSharedPointer<DisplayHandle> displayHandle) override;
    QWidget* controls(QSharedPointer<DisplayHandle> displayHandle) override;

private:
    void initialize(QSharedPointer<DisplayHandle> displayHandle);

    BitRasterWidget *m_displayWidget;
    BitRasterControls *m_controlsWidget;
};

#endif // BITRASTER_H
