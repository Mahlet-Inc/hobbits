#ifndef IMPORTEXPORTINTERFACE_H
#define IMPORTEXPORTINTERFACE_H

#include "bitcontainer.h"
#include "importexportresult.h"
#include <QSharedPointer>

#include "hobbits-core_global.h"

class HOBBITSCORESHARED_EXPORT ImportExportInterface
{
public:
    virtual ~ImportExportInterface()
    {
    }

    virtual ImportExportInterface* createDefaultImporterExporter() = 0;

    virtual QString getName() = 0;

    virtual bool canImport() = 0;
    virtual bool canExport() = 0;

    virtual QString getImportLabelForState(QJsonObject pluginState) = 0;
    virtual QString getExportLabelForState(QJsonObject pluginState) = 0;

    virtual QSharedPointer<ImportExportResult> importBits(QJsonObject pluginState, QWidget *parent) = 0;
    virtual QSharedPointer<ImportExportResult> exportBits(
            QSharedPointer<const BitContainer> container,
            QJsonObject pluginState,
            QWidget *parent) = 0;

};

Q_DECLARE_INTERFACE(ImportExportInterface, "hobbits.ImportExportInterface")

#endif // IMPORTEXPORTINTERFACE_H
