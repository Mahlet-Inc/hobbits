#ifndef PYTHONIMPORTER_H
#define PYTHONIMPORTER_H

#include "importexportinterface.h"
#include "parameterdelegate.h"
#include "pythonpluginconfig.h"
#include "hobbits-python_global.h"

class HOBBITSPYTHONSHARED_EXPORT PythonImporter : public ImporterExporterInterface
{
public:
    PythonImporter(QSharedPointer<PythonPluginConfig> config);

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
    QSharedPointer<PythonPluginConfig> m_config;
};

#endif // PYTHONIMPORTER_H
