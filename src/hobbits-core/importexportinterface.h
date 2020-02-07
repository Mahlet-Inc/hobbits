#ifndef IMPORTEXPORTINTERFACE_H
#define IMPORTEXPORTINTERFACE_H

#include "bitcontainer.h"
#include <QSharedPointer>

class ImportExportInterface
{
public:
    virtual ~ImportExportInterface()
    {
    }

    virtual ImportExportInterface* createDefaultImporterExporter() = 0;

    virtual QString getName() = 0;

    virtual bool canImport() = 0;
    virtual bool canExport() = 0;

    virtual QSharedPointer<BitContainer> importBits(QMap<QString, QString> args, QWidget *parent) = 0;
    virtual void exportBits(
            QSharedPointer<const BitContainer> container,
            QMap<QString, QString> args,
            QWidget *parent) = 0;

};

Q_DECLARE_INTERFACE(ImportExportInterface, "hobbits.ImportExportInterface")

#endif // IMPORTEXPORTINTERFACE_H
