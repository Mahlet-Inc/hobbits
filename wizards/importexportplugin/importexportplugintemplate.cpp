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

QSharedPointer<ImportExportResult> %{ClassName}::importBits(QJsonObject pluginState, QWidget *parent)
{
    return ImportExportResult::nullResult();
}

QSharedPointer<ImportExportResult> %{ClassName}::exportBits(
    QSharedPointer<const BitContainer> container,
    QJsonObject pluginState,
    QWidget *parent)
{
    return ImportExportResult::nullResult();
}
