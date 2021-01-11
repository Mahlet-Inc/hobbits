#ifndef %{JS: '%{ClassName}'.toUpperCase()}_H
#define %{JS: '%{ClassName}'.toUpperCase()}_H

#include "importexportinterface.h"
#include "parameterdelegate.h"

class %{ClassName} : public QObject, ImporterExporterInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "hobbits.ImporterExporterInterface.%{ClassName}")
    Q_INTERFACES(ImporterExporterInterface)

public:
    %{ClassName}();

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
    QSharedPointer<ParameterDelegate> m_importDelegate;
    QSharedPointer<ParameterDelegate> m_exportDelegate;
};

#endif // %{JS: '%{ClassName}'.toUpperCase()}_H
