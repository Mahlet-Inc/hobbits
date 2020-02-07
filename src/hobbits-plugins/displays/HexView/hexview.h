#ifndef HEXVIEW_H
#define HEXVIEW_H

#include "displayinterface.h"
#include "hexviewcontrols.h"
#include "hexviewwidget.h"


class HexView : public QObject, DisplayInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "hobbits.DisplayInterface.HexView")
    Q_INTERFACES(DisplayInterface)

public:
    HexView();

    DisplayInterface* createDefaultDisplay();

    QString getName();

    QWidget* getDisplayWidget(QSharedPointer<DisplayHandle> displayHandle);
    QWidget* getControlsWidget(QSharedPointer<DisplayHandle> displayHandle);

private:
    void initialize(QSharedPointer<DisplayHandle> displayHandle);

    HexViewWidget *m_displayWidget;
    HexViewControls *m_controlsWidget;
};

#endif // HEXVIEW_H
