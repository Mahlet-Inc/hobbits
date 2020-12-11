#ifndef HOBBITSPLUGIN_H
#define HOBBITSPLUGIN_H

#include "hobbits-core_global.h"
#include <QSharedPointer>
#include "parameterdelegate.h"
#include "pluginactionprogress.h"
#include "displayrenderconfig.h"


/**
  * @brief Abstract base plugin interface with common metadata methods
  *
  * \see AnalyzerInterface DisplayInterface ImportExportInterface OperatorInterface
*/
class HOBBITSCORESHARED_EXPORT HobbitsPlugin
{
public:
    virtual ~HobbitsPlugin() = default;

    virtual QString name() = 0;
    virtual QString description() = 0;
    virtual QStringList tags() = 0;
};

#endif // HOBBITSPLUGIN_H
