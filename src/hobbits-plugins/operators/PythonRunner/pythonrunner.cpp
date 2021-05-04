#include "pythonrunner.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QMetaObject>
#include <QObject>
#include <QProcess>
#include <QProcessEnvironment>
#include <QTemporaryDir>
#include <QStandardPaths>
#include "settingsmanager.h"
#include "hobbitspython.h"
#include "pythonrunnerform.h"

PythonRunner::PythonRunner()
{
    QList<ParameterDelegate::ParameterInfo> infos = {
        { "script", ParameterDelegate::ParameterType::String }
    };

    m_delegate = ParameterDelegateUi::create(
                    infos,
                    [](const QJsonObject &parameters) {
                        Q_UNUSED(parameters)
                        return QString("Python Run");
                    },
                    [](QSharedPointer<ParameterDelegate> delegate, QSize size) {
                        Q_UNUSED(size)
                        return new PythonRunnerForm(delegate);
                    });
}


OperatorInterface* PythonRunner::createDefaultOperator()
{
    return new PythonRunner();
}

QString PythonRunner::name()
{
    return "Python Runner";
}

QString PythonRunner::description()
{
    return "Run arbitrary Python code on the data using a Python API";
}

QStringList PythonRunner::tags()
{
    return {"Generic"};
}

QSharedPointer<ParameterDelegate> PythonRunner::parameterDelegate()
{
    return m_delegate;
}

int PythonRunner::getMinInputContainers(const QJsonObject &pluginState)
{
    Q_UNUSED(pluginState)
    return 1;
}

int PythonRunner::getMaxInputContainers(const QJsonObject &pluginState)
{
    Q_UNUSED(pluginState)
    return 1;
}

QSharedPointer<const OperatorResult> PythonRunner::operateOnBits(
        QList<QSharedPointer<const BitContainer>> inputContainers,
        const QJsonObject &parameters,
        QSharedPointer<PluginActionProgress> progressTracker)
{
    if (inputContainers.length() != 1) {
        return OperatorResult::error("Requires a single input bit container");
    }
    QStringList invalidations = m_delegate->validate(parameters);
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
    userScriptFile.write(parameters.value("script").toString().toLatin1());
    userScriptFile.close();

    auto outputBits = QSharedPointer<BitArray>(new BitArray());
    auto outputInfo = BitInfo::create(inputContainers.at(0)->bits()->sizeInBits());
    auto pyRequest = PythonRequest::create(userScriptFile.fileName())->setFunctionName("process_bits");
    pyRequest->addArg(PythonArg::constBitContainer(inputContainers.at(0)));
    pyRequest->addArg(PythonArg::bitArray(outputBits));
    pyRequest->addArg(PythonArg::bitInfo(outputInfo));
    auto watcher = HobbitsPython::getInstance().runProcessScript(pyRequest, progressTracker);
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
