#ifndef FILEDATA_H
#define FILEDATA_H

#include "importexportinterface.h"


namespace Ui
{
class FileData;
}

class FileData : public QObject, ImportExportInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "hobbits.ImportExportInterface.FileData")
    Q_INTERFACES(ImportExportInterface)

public:
    FileData();

    ~FileData();

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
    Ui::FileData *ui;
};

#endif // FILEDATA_H
