#ifndef HTTPDATA_H
#define HTTPDATA_H

#include "httptransceiver.h"
#include "importexportinterface.h"

class HttpData : public QObject, ImportExportInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "hobbits.ImportExportInterface.HttpData")
    Q_INTERFACES(ImportExportInterface)

public:
    HttpData();

    ~HttpData() override;

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
    HttpTransceiver *http;
};

#endif // HTTPDATA_H
