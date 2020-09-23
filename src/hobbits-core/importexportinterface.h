#ifndef IMPORTEXPORTINTERFACE_H
#define IMPORTEXPORTINTERFACE_H

#include "bitcontainer.h"
#include "exportresult.h"
#include "importresult.h"
#include <QSharedPointer>

#include "hobbits-core_global.h"

class HOBBITSCORESHARED_EXPORT ImportExportInterface
{
public:
    virtual ~ImportExportInterface() = default;

    virtual ImportExportInterface* createDefaultImporterExporter() = 0;

    virtual QString getName() = 0;

    virtual bool canImport() = 0;
    virtual bool canExport() = 0;

    virtual QString getImportLabelForState(QJsonObject pluginState) = 0;
    virtual QString getExportLabelForState(QJsonObject pluginState) = 0;

    virtual QSharedPointer<ImportResult> importBits(QJsonObject pluginState) = 0;
    virtual QSharedPointer<ExportResult> exportBits(
            QSharedPointer<const BitContainer> container,
            QJsonObject pluginState) = 0;

};

Q_DECLARE_INTERFACE(ImportExportInterface, "hobbits.ImportExportInterface")

#endif // IMPORTEXPORTINTERFACE_H
