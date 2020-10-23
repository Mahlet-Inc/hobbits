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

    DisplayInterface* createDefaultDisplay() override;

    QString name() override;
    QString description() override;
    QStringList tags() override;

    QWidget* display(QSharedPointer<DisplayHandle> displayHandle) override;
    QWidget* controls(QSharedPointer<DisplayHandle> displayHandle) override;

private:
    void initialize(QSharedPointer<DisplayHandle> displayHandle);

    HexViewWidget *m_displayWidget;
    HexViewControls *m_controlsWidget;
};

#endif // HEXVIEW_H
