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

    DisplayInterface* createDefaultDisplay() override;

    QString name() override;
    QString description() override;
    QStringList tags() override;

    QWidget* display(QSharedPointer<DisplayHandle> displayHandle) override;
    QWidget* controls(QSharedPointer<DisplayHandle> displayHandle) override;

private:
    void initialize(QSharedPointer<DisplayHandle> displayHandle);

    AsciiViewWidget *m_displayWidget;
    AsciiViewControls *m_controlsWidget;
};

#endif // ASCIIVIEW_H
