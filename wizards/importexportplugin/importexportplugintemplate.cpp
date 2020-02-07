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

QSharedPointer<BitContainer> %{ClassName}::importBits(QMap<QString, QString> args, QWidget *parent)
{
    QSharedPointer<BitContainer> nullContainer;
    return nullContainer;
}

void %{ClassName}::exportBits(
    QSharedPointer<const BitContainer> container,
    QMap<QString, QString> args,
    QWidget *parent)
{
}
