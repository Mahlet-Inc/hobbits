#ifndef UDPDATA_H
#define UDPDATA_H

#include "importexportinterface.h"

class UdpData : public QObject, ImportExportInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "hobbits.ImportExportInterface.UdpData")
    Q_INTERFACES(ImportExportInterface)

public:
    UdpData();
    ~UdpData() override;

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

#endif // UDPDATA_H
