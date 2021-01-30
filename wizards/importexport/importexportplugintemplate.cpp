#include "%{HeaderFileName}"
#include "%{ImportFormHeaderFileName}"
#include "%{ExportFormHeaderFileName}"

%{ClassName}::%{ClassName}() 
{
    QList<ParameterDelegate::ParameterInfo> importInfos = {
        // TODO: add parameters like {"myparametername", QJsonValue::Double}
    };

    m_importDelegate = ParameterDelegate::create(
                    importInfos,
                    [this](const QJsonObject &parameters) {
                        // TODO: use parameters to describe action better
                        return QString("%1 Import").arg(this->name());
                    },
                    [](QSharedPointer<ParameterDelegate> delegate, QSize size) {
                        Q_UNUSED(size)
                        return new %{ImportFormClassName}(delegate);
                    });

    QList<ParameterDelegate::ParameterInfo> exportInfos = {
        // TODO: add parameters like {"myparametername", QJsonValue::Double}
    };

    m_exportDelegate = ParameterDelegate::create(
                    exportInfos,
                    [this](const QJsonObject &parameters) {
                        // TODO: use parameters to describe action better
                        return QString("%1 Export").arg(this->name());
                    },
                    [](QSharedPointer<ParameterDelegate> delegate, QSize size) {
                        Q_UNUSED(size)
                        return new %{ExportFormClassName}(delegate);
                    });
}

ImporterExporterInterface* %{ClassName}::createDefaultImporterExporter()
{
    return new %{ClassName}();
}

QString %{ClassName}::name()
{
    return "%{ClassName}";
}

QString %{ClassName}::description()
{
    // TODO: create actual description
    return "Describes what %{ClassName} does";
}

QStringList %{ClassName}::tags()
{
    // TODO: add relevant tags
    return {"Generic"};
}

bool %{ClassName}::canExport()
{
    return true;
}

bool %{ClassName}::canImport()
{
    return true;
}

QSharedPointer<ParameterDelegate>  %{ClassName}::importParameterDelegate()
{
    return m_importDelegate;
}

QSharedPointer<ParameterDelegate>  %{ClassName}::exportParameterDelegate()
{
    return m_exportDelegate;
}

QSharedPointer<ImportResult> %{ClassName}::importBits(QJsonObject parameters,
                                                      QSharedPointer<PluginActionProgress> progress)
{
    if (!m_importDelegate->validate(parameters)) {
        return ImportResult::error(QString("Invalid parameters passed to %1").arg(name()));
    }

    return ImportResult::error("Import not implemented");
}

QSharedPointer<ExportResult> %{ClassName}::exportBits(QSharedPointer<const BitContainer> container,
                                                      QJsonObject parameters,
                                                      QSharedPointer<PluginActionProgress> progress)
{
    if (!m_exportDelegate->validate(parameters)) {
        return ExportResult::error(QString("Invalid parameters passed to %1").arg(name()));
    }

    return ExportResult::error("Export not implemented");
}
