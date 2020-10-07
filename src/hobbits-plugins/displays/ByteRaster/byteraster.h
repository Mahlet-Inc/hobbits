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

    DisplayInterface* createDefaultDisplay() override;

    QString name() override;
    QString description() override;
    QStringList tags() override;

    QWidget* display(QSharedPointer<DisplayHandle> displayHandle) override;
    QWidget* controls(QSharedPointer<DisplayHandle> displayHandle) override;

private:
    void initialize(QSharedPointer<DisplayHandle> displayHandle);

    ByteRasterWidget *m_displayWidget;
    ByteRasterControls *m_controlsWidget;
};

#endif // BYTERASTER_H
