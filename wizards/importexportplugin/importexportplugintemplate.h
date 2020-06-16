#ifndef %{JS: '%{ClassName}'.toUpperCase()}_H
#define %{JS: '%{ClassName}'.toUpperCase()}_H

#include "importexportinterface.h"

class %{ClassName} : public QObject, ImportExportInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "hobbits.ImportExportInterface.%{ClassName}")
    Q_INTERFACES(ImportExportInterface)

public:
    %{ClassName}();
    ~%{ClassName}() override;

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

#endif // %{JS: '%{ClassName}'.toUpperCase()}_H
