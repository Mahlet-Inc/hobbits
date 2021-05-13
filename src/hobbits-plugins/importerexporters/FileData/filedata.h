#ifndef FILEDATA_H
#define FILEDATA_H

#include "importexportinterface.h"
#include "parameterdelegate.h"

class FileData : public QObject, ImporterExporterInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "hobbits.ImporterExporterInterface.FileData")
    Q_INTERFACES(ImporterExporterInterface)

public:
    FileData();

    ~FileData() override;

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

#endif // FILEDATA_H
