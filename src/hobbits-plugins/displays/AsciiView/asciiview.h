#ifndef ASCIIVIEW_H
#define ASCIIVIEW_H

#include "asciiviewcontrols.h"
#include "asciiviewwidget.h"
#include "displayinterface.h"


class AsciiView : public QObject, DisplayInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "hobbits.DisplayInterface.AsciiView")
    Q_INTERFACES(DisplayInterface)

public:
    AsciiView();

    DisplayInterface* createDefaultDisplay();

    QString getName();

    QWidget* getDisplayWidget(QSharedPointer<DisplayHandle> displayHandle);
    QWidget* getControlsWidget(QSharedPointer<DisplayHandle> displayHandle);

private:
    void initialize(QSharedPointer<DisplayHandle> displayHandle);

    AsciiViewWidget *m_displayWidget;
    AsciiViewControls *m_controlsWidget;
};

#endif // ASCIIVIEW_H
