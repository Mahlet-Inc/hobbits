#include "pythonexporter.h"
#include <QTemporaryDir>
#include "hobbitspython.h"

PythonExporter::PythonExporter(QSharedPointer<PythonPluginConfig> config) :
    m_config(config)
{

}

ImporterExporterInterface *PythonExporter::createDefaultImporterExporter()
{
    return new PythonExporter(m_config);
}

QString PythonExporter::name()
{
    return m_config->name();
}

QString PythonExporter::description()
{
    return m_config->description();
}

QStringList PythonExporter::tags()
{
    return m_config->tags();
}

bool PythonExporter::canExport()
{
    return true;
}

bool PythonExporter::canImport()
{
    return false;
}

QSharedPointer<ParameterDelegate> PythonExporter::importParameterDelegate()
{
    return nullptr;
}

QSharedPointer<ParameterDelegate> PythonExporter::exportParameterDelegate()
{
    return m_config->delegate();
}

QSharedPointer<ImportResult> PythonExporter::importBits(QJsonObject parameters, QSharedPointer<PluginActionProgress> progress)
{
    Q_UNUSED(parameters)
    Q_UNUSED(progress)
    return ImportResult::error("Import not implemented");
}

QSharedPointer<ExportResult> PythonExporter::exportBits(QSharedPointer<const BitContainer> container, QJsonObject parameters, QSharedPointer<PluginActionProgress> progress)
{
    QStringList invalidations = m_config->delegate()->validate(parameters);
    if (!invalidations.isEmpty()) {
        return ExportResult::error(QString("Invalid parameters passed to %1:\n%2").arg(name()).arg(invalidations.join("\n")));
    }

    QTemporaryDir dir;
    if (!dir.isValid()) {
        return ExportResult::error("Could not create temporary directory");
    }

    QFile userScriptFile(dir.filePath("user_script.py"));
    if (!userScriptFile.open(QIODevice::Truncate | QIODevice::WriteOnly)) {
        return ExportResult::error("Could not write script to temporary directory");
    }
    userScriptFile.write(m_config->script().toLatin1());
    userScriptFile.close();

    auto outputBits = QSharedPointer<BitArray>(new BitArray());
    auto outputInfo = BitInfo::create();
    auto pyRequest = PythonRequest::create(userScriptFile.fileName())->setFunctionName("export_bits");
    for (auto extraPath: m_config->extraPaths()) {
        pyRequest->addPathExtension(extraPath);
    }
    pyRequest->addArg(PythonArg::constBitContainer(container));
    for (auto param : m_config->parameterInfos()) {
        if (param.type == ParameterDelegate::ParameterType::String) {
            pyRequest->addArg(PythonArg::qString(parameters.value(param.name).toString()));
        }
        else if (param.type == ParameterDelegate::ParameterType::Integer) {
            pyRequest->addArg(PythonArg::integer(parameters.value(param.name).toInt()));
        }
        else if (param.type == ParameterDelegate::ParameterType::Decimal) {
            pyRequest->addArg(PythonArg::number(parameters.value(param.name).toDouble()));
        }
        else if (param.type == ParameterDelegate::ParameterType::Boolean) {
            pyRequest->addArg(PythonArg::boolean(parameters.value(param.name).toBool()));
        }
    }

    auto watcher = HobbitsPython::getInstance().runProcessScript(pyRequest, progress);
    watcher->watcher()->future().waitForFinished();
    auto result = watcher->result();

    QString output = "";
    bool error = false;
    if (!result->getStdOut().isEmpty()) {
        output += "Python stdout:\n" + result->getStdOut() + "\n\n";
    }
    if (!result->getStdErr().isEmpty()) {
        error = true;
        output += "Python stderr:\n" + result->getStdErr() + "\n\n";
    }
    if (!result->errors().isEmpty()) {
        error = true;
        output += "Other errors:\n" + result->errors().join("\n") + "\n\n";
    }
    if (error) {
        return ExportResult::error(output);
    }

    return ExportResult::result(parameters);
}
