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

    QString getImportLabelForState(QJsonObject pluginState) override;
    QString getExportLabelForState(QJsonObject pluginState) override;

    QSharedPointer<ImportExportResult> importBits(QJsonObject pluginState, QWidget *parent) override;
    QSharedPointer<ImportExportResult> exportBits(
            QSharedPointer<const BitContainer> container,
            QJsonObject pluginState,
            QWidget *parent) override;

private:
};

#endif // HEXSTRING_H
