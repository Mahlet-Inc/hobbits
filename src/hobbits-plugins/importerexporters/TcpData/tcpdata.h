#ifndef TCPDATA_H
#define TCPDATA_H

#include "importexportinterface.h"

class TcpData : public QObject, ImporterExporterInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "hobbits.ImportExportInterface.TcpData")
    Q_INTERFACES(ImporterExporterInterface)

public:
    TcpData();
    ~TcpData() override;

    ImporterExporterInterface* createDefaultImporterExporter() override;

    QString name() override;

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
