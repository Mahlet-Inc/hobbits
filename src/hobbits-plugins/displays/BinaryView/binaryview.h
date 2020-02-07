#ifndef BINARYVIEW_H
#define BINARYVIEW_H

#include "binaryviewcontrols.h"
#include "binaryviewwidget.h"
#include "displayinterface.h"


class BinaryView : public QObject, DisplayInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "hobbits.DisplayInterface.BinaryView")
    Q_INTERFACES(DisplayInterface)

public:
    BinaryView();

    DisplayInterface* createDefaultDisplay();

    QString getName();

    QWidget* getDisplayWidget(QSharedPointer<DisplayHandle> displayHandle);
    QWidget* getControlsWidget(QSharedPointer<DisplayHandle> displayHandle);

private:
    void initialize(QSharedPointer<DisplayHandle> displayHandle);

    BinaryViewWidget *m_displayWidget;
    BinaryViewControls *m_controlsWidget;
};

#endif // BINARYVIEW_H
