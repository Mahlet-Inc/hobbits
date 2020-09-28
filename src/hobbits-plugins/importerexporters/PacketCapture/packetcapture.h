#ifndef PACKETCAPTURE_H
#define PACKETCAPTURE_H

#include "importexportinterface.h"

class PacketCapture : public QObject, ImportExportInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "hobbits.ImportExportInterface.PacketCapture")
    Q_INTERFACES(ImportExportInterface)

public:
    PacketCapture();
    ~PacketCapture() override;

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

#endif // PACKETCAPTURE_H
