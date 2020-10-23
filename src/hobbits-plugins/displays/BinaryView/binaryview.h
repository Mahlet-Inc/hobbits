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

    DisplayInterface* createDefaultDisplay() override;

    QString name() override;
    QString description() override;
    QStringList tags() override;

    QWidget* display(QSharedPointer<DisplayHandle> displayHandle) override;
    QWidget* controls(QSharedPointer<DisplayHandle> displayHandle) override;

private:
    void initialize(QSharedPointer<DisplayHandle> displayHandle);

    BinaryViewWidget *m_displayWidget;
    BinaryViewControls *m_controlsWidget;
};

#endif // BINARYVIEW_H
