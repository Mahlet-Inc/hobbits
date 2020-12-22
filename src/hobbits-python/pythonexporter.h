#ifndef PYTHONEXPORTER_H
#define PYTHONEXPORTER_H

#include "importexportinterface.h"
#include "parameterdelegate.h"
#include "pythonpluginconfig.h"
#include "hobbits-python_global.h"

class PythonExporter : public ImporterExporterInterface
{
public:
    PythonExporter(QSharedPointer<PythonPluginConfig> config);

    ImporterExporterInterface* createDefaultImporterExporter() override;

    QString name() override;
    QString description() override;
    QStringList tags() override;

    bool canExport() override;
    bool canImport() override;

    virtual QSharedPointer<ParameterDelegate> importParameterDelegate() override;
    virtual QSharedPointer<ParameterDelegate> exportParameterDelegate() override;

    QSharedPointer<ImportResult> importBits(QJsonObject parameters,
                                            QSharedPointer<PluginActionProgress> progress) override;
    QSharedPointer<ExportResult> exportBits(QSharedPointer<const BitContainer> container,
                                            QJsonObject parameters,
                                            QSharedPointer<PluginActionProgress> progress) override;

private:
    QSharedPointer<PythonPluginConfig> m_config;
};

#endif // PYTHONEXPORTER_H
