#include "pythonoperator.h"
#include <QTemporaryDir>
#include "hobbitspython.h"


PythonOperator::PythonOperator(QSharedPointer<PythonPluginConfig> config) :
    m_config(config)
{

}

OperatorInterface *PythonOperator::createDefaultOperator()
{
    return new PythonOperator(m_config);
}

QString PythonOperator::name()
{
    return m_config->name();
}

QString PythonOperator::description()
{
    return m_config->description();
}

QStringList PythonOperator::tags()
{
    return m_config->tags();
}

QSharedPointer<ParameterDelegate> PythonOperator::parameterDelegate()
{
    return m_config->delegate();
}

int PythonOperator::getMinInputContainers(const QJsonObject &pluginState)
{
    Q_UNUSED(pluginState)
    return 1;
}

int PythonOperator::getMaxInputContainers(const QJsonObject &pluginState)
{
    Q_UNUSED(pluginState)
    return 1;
}

QSharedPointer<const OperatorResult> PythonOperator::operateOnBits(QList<QSharedPointer<const BitContainer> > inputContainers, const QJsonObject &parameters, QSharedPointer<PluginActionProgress> progress)
{

    if (inputContainers.length() != 1) {
        return OperatorResult::error("Requires a single input bit container");
    }
    QStringList invalidations = m_config->delegate()->validate(parameters);
    if (!invalidations.isEmpty()) {
        return OperatorResult::error(QString("Invalid parameters passed to %1:\n%2").arg(name()).arg(invalidations.join("\n")));
    }

    QTemporaryDir dir;
    if (!dir.isValid()) {
        return OperatorResult::error("Could not create temporary directory");
    }

    QFile userScriptFile(dir.filePath("user_script.py"));
    if (!userScriptFile.open(QIODevice::Truncate | QIODevice::WriteOnly)) {
        return OperatorResult::error("Could not write script to temporary directory");
    }
    userScriptFile.write(m_config->script().toLatin1());
    userScriptFile.close();

    auto outputBits = QSharedPointer<BitArray>(new BitArray());
    auto outputInfo = BitInfo::create(inputContainers.at(0)->bits()->sizeInBits());
    auto pyRequest = PythonRequest::create(userScriptFile.fileName())->setFunctionName("operate_on_bits");
    for (auto extraPath: m_config->extraPaths()) {
        pyRequest->addPathExtension(extraPath);
    }
    pyRequest->addArg(PythonArg::constBitContainer(inputContainers.at(0)));
    pyRequest->addArg(PythonArg::bitArray(outputBits));
    pyRequest->addArg(PythonArg::bitInfo(outputInfo));
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
        return OperatorResult::error(output);
    }

    outputInfo->setMetadata(name() + " Run Output", output);

    QSharedPointer<BitContainer> outputContainer = BitContainer::create(outputBits);
    outputContainer->setInfo(outputInfo);
    outputContainer->setName(QString("python <- %1").arg(inputContainers.at(0)->name()));

    return OperatorResult::result({outputContainer}, parameters);
}
