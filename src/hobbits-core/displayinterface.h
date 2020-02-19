#ifndef DISPLAYINTERFACE_H
#define DISPLAYINTERFACE_H

#include <QSharedPointer>
#include <QStringListModel>

#include "bitcontainer.h"
#include "displayhandle.h"

#include "hobbits-core_global.h"

class HOBBITSCORESHARED_EXPORT DisplayInterface
{
public:
    virtual ~DisplayInterface()
    {
    }

    virtual DisplayInterface* createDefaultDisplay() = 0;

    virtual QString getName() = 0;

    virtual QWidget* getDisplayWidget(QSharedPointer<DisplayHandle> displayHandle) = 0;
    virtual QWidget* getControlsWidget(QSharedPointer<DisplayHandle> displayHandle) = 0;

};

Q_DECLARE_INTERFACE(DisplayInterface, "hobbits.DisplayInterface")

#endif // DISPLAYINTERFACE_H
