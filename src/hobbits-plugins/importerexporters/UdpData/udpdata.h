#ifndef UDPDATA_H
#define UDPDATA_H

#include "parameterdelegate.h"
#include "importexportinterface.h"

class UdpData : public QObject, ImporterExporterInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "hobbits.ImporterExporterInterface.UdpData")
    Q_INTERFACES(ImporterExporterInterface)

public:
    UdpData();

    ImporterExporterInterface* createDefaultImporterExporter() override;

    QString name() override;
    QString description() override;
    QStringList tags() override;

    bool canExport() override;
    bool canImport() override;

    virtual QSharedPointer<ParameterDelegate> importParameterDelegate() override;
    virtual QSharedPointer<ParameterDelegate> exportParameterDelegate() override;

    QSharedPointer<ImportResult> importBits(const Parameters &parameters,
                                            QSharedPointer<PluginActionProgress> progress) override;
    QSharedPointer<ExportResult> exportBits(QSharedPointer<const BitContainer> container,
                                            const Parameters &parameters,
                                            QSharedPointer<PluginActionProgress> progress) override;

private:
    QSharedPointer<ParameterDelegate> m_importDelegate;
    QSharedPointer<ParameterDelegate> m_exportDelegate;
};

#endif // UDPDATA_H
