#ifndef TCPDATA_H
#define TCPDATA_H

#include "importexportinterface.h"

class TcpData : public QObject, ImportExportInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "hobbits.ImportExportInterface.TcpData")
    Q_INTERFACES(ImportExportInterface)

public:
    TcpData();
    ~TcpData() override;

    ImportExportInterface* createDefaultImporterExporter() override;

    QString getName() override;

    bool canExport() override;
    bool canImport() override;

    QString getImportLabelForState(QJsonObject pluginState) override;
    QString getExportLabelForState(QJsonObject pluginState) override;

    QSharedPointer<ImportResult> importBits(QJsonObject pluginState) override;
    QSharedPointer<ExportResult> exportBits(
            QSharedPointer<const BitContainer> container,
            QJsonObject pluginState) override;

private:
};

#endif // TCPDATA_H
