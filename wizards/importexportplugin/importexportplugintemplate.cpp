#include "%{HeaderFileName}"

%{ClassName}::%{ClassName}() 
{

}

%{ClassName}::~%{ClassName}()
{
}

ImportExportInterface* %{ClassName}::createDefaultImporterExporter()
{
    return new %{ClassName}();
}

QString %{ClassName}::getName()
{
    return "%{ClassName}";
}

bool %{ClassName}::canExport()
{
    return true;
}

bool %{ClassName}::canImport()
{
    return true;
}

QString %{ClassName}::getImportLabelForState(QJsonObject pluginState)
{
    return "";
}

QString %{ClassName}::getExportLabelForState(QJsonObject pluginState)
{
    return "";
}

QSharedPointer<ImportResult> %{ClassName}::importBits(QJsonObject pluginState)
{
    return ImportResult::error("Import not implemented");
}

QSharedPointer<ExportResult> %{ClassName}::exportBits(
    QSharedPointer<const BitContainer> container,
    QJsonObject pluginState)
{
    return ExportResult::error("Export not implemented");
}
