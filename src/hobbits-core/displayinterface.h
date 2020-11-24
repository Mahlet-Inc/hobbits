#ifndef DISPLAYINTERFACE_H
#define DISPLAYINTERFACE_H

#include "hobbits-core_global.h"
#include <QSharedPointer>
#include "parameterdelegate.h"
#include "pluginactionprogress.h"
#include "displayrenderconfig.h"

class QWidget;
class DisplayHandle;

/**
  * @brief Implementations of the DisplayInterface plugin interface display BitContainer information
  *
  * \see BitContainer DisplayHandle
*/
class HOBBITSCORESHARED_EXPORT DisplayInterface
{
public:
    virtual ~DisplayInterface() = default;

    virtual DisplayInterface* createDefaultDisplay() = 0;

    virtual QString name() = 0;
    virtual QString description() = 0;
    virtual QStringList tags() = 0;

    virtual QSharedPointer<DisplayRenderConfig> renderConfig() = 0;
    virtual void setDisplayHandle(QSharedPointer<DisplayHandle> displayHandle) = 0;
    virtual QSharedPointer<ParameterDelegate> parameterDelegate() = 0;

    virtual QImage renderDisplay(
            QSize viewportSize,
            const QJsonObject &parameters,
            QSharedPointer<PluginActionProgress> progress) = 0;

    virtual QImage renderOverlay(
            QSize viewportSize,
            const QJsonObject &parameters) = 0;
};

Q_DECLARE_INTERFACE(DisplayInterface, "hobbits.DisplayInterface")

#endif // DISPLAYINTERFACE_H
