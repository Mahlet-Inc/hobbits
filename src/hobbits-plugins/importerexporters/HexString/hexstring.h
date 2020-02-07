#ifndef HEXSTRING_H
#define HEXSTRING_H

#include "importexportinterface.h"


class HexString : public QObject, ImportExportInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "hobbits.ImportExportInterface.HexString")
    Q_INTERFACES(ImportExportInterface)

public:
    HexString();

    ~HexString();

    ImportExportInterface* createDefaultImporterExporter() override;

    QString getName() override;

    bool canExport() override;
    bool canImport() override;

    QSharedPointer<BitContainer> importBits(QMap<QString, QString> args, QWidget *parent) override;
    void exportBits(
            QSharedPointer<const BitContainer> container,
            QMap<QString, QString> args,
            QWidget *parent) override;

private:
};

#endif // HEXSTRING_H
