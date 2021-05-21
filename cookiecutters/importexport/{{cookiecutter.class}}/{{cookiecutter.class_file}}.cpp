#include "{{cookiecutter.class|lower}}.h"
#include "{{cookiecutter.import_editor|lower}}.h"
#include "{{cookiecutter.export_editor|lower}}.h"

{{cookiecutter.class}}::{{cookiecutter.class}}() 
{
    QList<ParameterDelegate::ParameterInfo> importInfos = {
        // TODO: add parameters like {"myparametername", ParameterDelegate::ParameterType::Decimal}
    };

    m_importDelegate = ParameterDelegate::create(
                    importInfos,
                    [this](const Parameters &parameters) {
                        // TODO: use parameters to describe action better
                        return QString("%1 Import").arg(this->name());
                    },
                    [](QSharedPointer<ParameterDelegate> delegate, QSize size) {
                        Q_UNUSED(size)
                        return new {{cookiecutter.import_editor}}(delegate);
                    });

    QList<ParameterDelegate::ParameterInfo> exportInfos = {
        // TODO: add parameters like {"myparametername", ParameterDelegate::ParameterType::Decimal}
    };

    m_exportDelegate = ParameterDelegate::create(
                    exportInfos,
                    [this](const Parameters &parameters) {
                        // TODO: use parameters to describe action better
                        return QString("%1 Export").arg(this->name());
                    },
                    [](QSharedPointer<ParameterDelegate> delegate, QSize size) {
                        Q_UNUSED(size)
                        return new {{cookiecutter.export_editor}}(delegate);
                    });
}

ImporterExporterInterface* {{cookiecutter.class}}::createDefaultImporterExporter()
{
    return new {{cookiecutter.class}}();
}

QString {{cookiecutter.class}}::name()
{
    return "{{cookiecutter.class}}";
}

QString {{cookiecutter.class}}::description()
{
    // TODO: create actual description
    return "Describes what {{cookiecutter.class}} does";
}

QStringList {{cookiecutter.class}}::tags()
{
    // TODO: add relevant tags
    return {"Generic"};
}

bool {{cookiecutter.class}}::canExport()
{
    return true;
}

bool {{cookiecutter.class}}::canImport()
{
    return true;
}

QSharedPointer<ParameterDelegate>  {{cookiecutter.class}}::importParameterDelegate()
{
    return m_importDelegate;
}

QSharedPointer<ParameterDelegate>  {{cookiecutter.class}}::exportParameterDelegate()
{
    return m_exportDelegate;
}

QSharedPointer<ImportResult> {{cookiecutter.class}}::importBits(Parameters parameters,
                                                      QSharedPointer<PluginActionProgress> progress)
{
    QStringList invalidations = m_importDelegate->validate(parameters);
    if (!invalidations.isEmpty()) {
        return ImportResult::error(QString("Invalid parameters passed to %1:\n%2").arg(name()).arg(invalidations.join("\n")));
    }

    return ImportResult::error("Import not implemented");
}

QSharedPointer<ExportResult> {{cookiecutter.class}}::exportBits(QSharedPointer<const BitContainer> container,
                                                      Parameters parameters,
                                                      QSharedPointer<PluginActionProgress> progress)
{
    QStringList invalidations = m_exportDelegate->validate(parameters);
    if (!invalidations.isEmpty()) {
        return ExportResult::error(QString("Invalid parameters passed to %1:\n%2").arg(name()).arg(invalidations.join("\n")));
    }

    return ExportResult::error("Export not implemented");
}
