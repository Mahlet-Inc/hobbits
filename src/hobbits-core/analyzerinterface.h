#ifndef ANALYZERINTERFACE_H
#define ANALYZERINTERFACE_H

#include <QSharedPointer>
#include "pluginactionprogress.h"
#include "analyzerresult.h"
#include "bitcontainer.h"
#include "bitcontainerpreview.h"
#include "parameterdelegate.h"
#include "hobbitsplugin.h"
#include "parameters.h"

/**
  * @brief Implementations of the AnalyzerInterface plugin interface enrich BitContainer metadata
  *
  * Analyzer plugins do not change any of the bits in a BitContainer. Instead, they analyze the
  * bits and existing metadata in order to provide more/better metadata for subsequent plugins
  * or manual analysis.
  *
  * Since Analyzers work with read-only BitContainer objects, they do not require any new copies of
  * the bits to be created. This is different from the OperatorInterface in which the results
  * comprise of newly created BitContainer instances.
  *
  * \see ParameterDelegate AnalyzerResult BitContainer ActionProgress
*/
class HOBBITSCORESHARED_EXPORT AnalyzerInterface : public virtual HobbitsPlugin
{
public:
    virtual ~AnalyzerInterface() = default;

    virtual AnalyzerInterface* createDefaultAnalyzer() = 0;

    virtual QSharedPointer<ParameterDelegate> parameterDelegate() = 0;

    virtual QSharedPointer<const AnalyzerResult> analyzeBits(
            QSharedPointer<const BitContainer> container,
            const Parameters &parameters,
            QSharedPointer<PluginActionProgress> progress) = 0;
};

Q_DECLARE_INTERFACE(AnalyzerInterface, "hobbits.AnalyzerInterface")

#endif // ANALYZERINTERFACE_H
