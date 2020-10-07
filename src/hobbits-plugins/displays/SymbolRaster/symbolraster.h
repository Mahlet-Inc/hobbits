#ifndef SYMBOLRASTER_H
#define SYMBOLRASTER_H

#include "displayinterface.h"
#include "symbolrastercontrols.h"
#include "symbolrasterwidget.h"


class SymbolRaster : public QObject, DisplayInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "hobbits.DisplayInterface.SymbolRaster")
    Q_INTERFACES(DisplayInterface)

public:
    SymbolRaster();

    DisplayInterface* createDefaultDisplay() override;

    QString name() override;
    QString description() override;
    QStringList tags() override;

    QWidget* display(QSharedPointer<DisplayHandle> displayHandle) override;
    QWidget* controls(QSharedPointer<DisplayHandle> displayHandle) override;

private:
    void initialize(QSharedPointer<DisplayHandle> displayHandle);

    SymbolRasterWidget *m_displayWidget;
    SymbolRasterControls *m_controlsWidget;
};

#endif // SYMBOLRASTER_H
