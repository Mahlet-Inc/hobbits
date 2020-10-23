#ifndef IMPORTEXPORTINTERFACE_H
#define IMPORTEXPORTINTERFACE_H

#include "bitcontainer.h"
#include "exportresult.h"
#include "importresult.h"
#include "parameterdelegate.h"
#include <QSharedPointer>
#include "pluginactionprogress.h"
#include "hobbits-core_global.h"

/**
  * @brief Implementations of the ImportExportInterface plugin interface import and/or export BitContainers
  *
  * Analyzer plugins do not change any of the bits in a BitContainer. Instead, they analyze the
  * bits and existing metadata in order to provide more/better metadata for subsequent plugins
  * or manual analysis.
  *
  * \see ParameterDelegate ImportResult ExportResult BitContainer ActionProgress
*/
class HOBBITSCORESHARED_EXPORT ImporterExporterInterface
{
public:
    virtual ~ImporterExporterInterface() = default;

    virtual ImporterExporterInterface* createDefaultImporterExporter() = 0;

    virtual QString name() = 0;
    virtual QString description() = 0;
    virtual QStringList tags() = 0;

    virtual bool canImport() = 0;
    virtual bool canExport() = 0;

    virtual QSharedPointer<ParameterDelegate> importParameterDelegate() = 0;
    virtual QSharedPointer<ParameterDelegate> exportParameterDelegate() = 0;

    virtual QSharedPointer<ImportResult> importBits(QJsonObject parameters,
                                                    QSharedPointer<PluginActionProgress> progress) = 0;
    virtual QSharedPointer<ExportResult> exportBits(QSharedPointer<const BitContainer> container,
                                                    QJsonObject parameters,
                                                    QSharedPointer<PluginActionProgress> progress) = 0;
};

Q_DECLARE_INTERFACE(ImporterExporterInterface, "hobbits.ImporterExporterInterface")

#endif // IMPORTEXPORTINTERFACE_H
