#include "pythonanalyzer.h"
#include <QTemporaryDir>
#include "hobbitspython.h"

PythonAnalyzer::PythonAnalyzer(QSharedPointer<PythonPluginConfig> config) :
    m_config(config)
{

}

AnalyzerInterface *PythonAnalyzer::createDefaultAnalyzer()
{
    return new PythonAnalyzer(m_config);
}

QString PythonAnalyzer::name()
{
    return m_config->name();
}

QString PythonAnalyzer::description()
{
    return m_config->description();
}

QStringList PythonAnalyzer::tags()
{
    return m_config->tags();
}

QSharedPointer<ParameterDelegate> PythonAnalyzer::parameterDelegate()
{
    return m_config->delegate();
}

QSharedPointer<const AnalyzerResult> PythonAnalyzer::analyzeBits(QSharedPointer<const BitContainer> container,
                                                                 const QJsonObject &parameters,
                                                                 QSharedPointer<PluginActionProgress> progress)
{
    if (!m_config->delegate()->validate(parameters)) {
        return AnalyzerResult::error("Invalid plugin parameters");
    }

    QTemporaryDir dir;
    if (!dir.isValid()) {
        return AnalyzerResult::error("Could not create temporary directory");
    }

    QFile userScriptFile(dir.filePath("user_script.py"));
    if (!userScriptFile.open(QIODevice::Truncate | QIODevice::WriteOnly)) {
        return AnalyzerResult::error("Could not write script to temporary directory");
    }
    userScriptFile.write(m_config->script().toLatin1());
    userScriptFile.close();

    auto outputInfo = BitInfo::create();
    auto pyRequest = PythonRequest::create(userScriptFile.fileName())->setFunctionName("analyze_bits");
    for (auto extraPath: m_config->extraPaths()) {
        pyRequest->addPathExtension(extraPath);
    }
    pyRequest->addArg(PythonArg::constBitContainer(container));
    pyRequest->addArg(PythonArg::bitInfo(outputInfo));
    for (auto param : m_config->parameterInfos()) {
        if (param.type == QJsonValue::String) {
            pyRequest->addArg(PythonArg::qString(parameters.value(param.name).toString()));
        }
        else if (param.type == QJsonValue::Double) {
            if (m_config->parameterNumberType(param.name) == PythonPluginConfig::Integer) {
                pyRequest->addArg(PythonArg::integer(parameters.value(param.name).toInt()));
            }
            else {
                pyRequest->addArg(PythonArg::number(parameters.value(param.name).toDouble()));
            }
        }
        else if (param.type == QJsonValue::Bool) {
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
        return AnalyzerResult::error(output);
    }

    outputInfo->setMetadata(name() + " Run Output", output);

    return AnalyzerResult::result(outputInfo, parameters);
}
