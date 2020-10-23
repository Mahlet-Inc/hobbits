#ifndef METADATA_H
#define METADATA_H

#include "displayinterface.h"
#include "metadatawidget.h"

class Metadata : public QObject, DisplayInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "hobbits.DisplayInterface.Metadata")
    Q_INTERFACES(DisplayInterface)

public:
    Metadata();

    DisplayInterface* createDefaultDisplay() override;

    QString name() override;
    QString description() override;
    QStringList tags() override;

    QWidget* display(QSharedPointer<DisplayHandle> displayHandle) override;
    QWidget* controls(QSharedPointer<DisplayHandle> displayHandle) override;

private:
    void initialize(QSharedPointer<DisplayHandle> displayHandle);
    MetadataWidget* m_displayWidget;
};

#endif // METADATA_H
