#ifndef DISPLAYINTERFACE_H
#define DISPLAYINTERFACE_H

#include "hobbitsplugin.h"
#include <QSharedPointer>
#include "parameterdelegate.h"
#include "pluginactionprogress.h"
#include "displayrenderconfig.h"
#include "parameters.h"

class QWidget;
class DisplayHandle;
class DisplayResult;

/**
  * @brief Implementations of the DisplayInterface plugin interface display BitContainer information
  *
  * \see BitContainer DisplayHandle
*/
class HOBBITSCORESHARED_EXPORT DisplayInterface : public virtual HobbitsPlugin
{
public:
    virtual ~DisplayInterface() = default;

    virtual DisplayInterface* createDefaultDisplay() = 0;

    virtual QSharedPointer<DisplayRenderConfig> renderConfig() = 0;
    virtual void setDisplayHandle(QSharedPointer<DisplayHandle> displayHandle) = 0;
    virtual QSharedPointer<ParameterDelegate> parameterDelegate() = 0;

    virtual QSharedPointer<DisplayResult> renderDisplay(
                QSize viewportSize,
                const Parameters &parameters,
                QSharedPointer<PluginActionProgress> progress) = 0;

    virtual QSharedPointer<DisplayResult> renderOverlay(
                QSize viewportSize,
                const Parameters &parameters) = 0;
};

Q_DECLARE_INTERFACE(DisplayInterface, "hobbits.DisplayInterface")

#endif // DISPLAYINTERFACE_H
