#include "pythonrunner.h"
#include "ui_pythonrunner.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QMetaObject>
#include <QObject>
#include <QProcess>
#include <QProcessEnvironment>
#include <QTemporaryDir>

#include "pythonsyntaxhighlighter.h"

#include "settingsmanager.h"

const QString PYTHON_PATH_KEY = "python_runner_path";

PythonRunner::PythonRunner() :
    ui(new Ui::PythonRunner())
{

}

// Return name of operator
QString PythonRunner::getName()
{
    return "Python Runner";
}

void PythonRunner::provideCallback(QSharedPointer<PluginCallback> pluginCallback)
{
    // the plugin callback allows the self-triggering of operateOnContainers
    m_pluginCallback = pluginCallback;
}

void PythonRunner::applyToWidget(QWidget *widget)
{
    ui->setupUi(widget);

    ui->te_pluginOutput->hide();
    connect(ui->pb_pythonPathSelect, SIGNAL(pressed()), this, SLOT(openPythonPathDialog()));
    ui->le_pythonPath->setText(SettingsManager::getInstance().getPrivateSetting(PYTHON_PATH_KEY).toString());

    connect(ui->pb_scriptHelp, SIGNAL(pressed()), this, SLOT(openHelpDialog()));

    new PythonSyntaxHighlighter(ui->te_pythonScript->document());
}

void PythonRunner::openPythonPathDialog()
{
    QString fileName = QFileDialog::getOpenFileName(
            nullptr,
            tr("Select Python"),
            SettingsManager::getInstance().getPrivateSetting(PYTHON_PATH_KEY).toString(),
            tr("Python (python*)"));
    if (fileName.isEmpty()) {
        return;
    }
    ui->le_pythonPath->setText(fileName);
    SettingsManager::getInstance().setPrivateSetting(PYTHON_PATH_KEY, fileName);
}

void PythonRunner::openHelpDialog()
{
    QMessageBox msg;
    msg.setWindowTitle("Python Runner API");
    QString docs;
    docs += "BitContainer.<strong>length</strong><br/>";
    docs += "The length of the container in bits<br/><br/>";
    docs += "BitContainer.<strong>at</strong>(<em>position</em>)<br/>";
    docs += "Gets the bit value at the given position<br/><br/>";
    docs += "BitContainer.<strong>set_at</strong>(<em>position</em>, <em>value</em>)<br/>";
    docs += "Sets the bit value at the given position<br/><br/>";
    docs += "BitContainer.<strong>set_length</strong>(<em>length</em>)<br/>";
    docs += "Sets the container's length in bits, initializing it with all 0s<br/><br/>";
    docs += "BitContainer.<strong>set_data</strong>(<em>bytes</em>, <em>length</em>)<br/>";
    docs += "Initializes the container with the given bit-packed bytes and the given length in bits<br/><br/>";
    docs += "BitContainer.<strong>bytes</strong><br/>";
    docs += "The bit-packed bytes of the container (use at own risk)";
    msg.setTextFormat(Qt::TextFormat::RichText);
    msg.setText(docs);
    msg.setDefaultButton(QMessageBox::Ok);
    msg.exec();
}

QJsonObject PythonRunner::getStateFromUi()
{
    QJsonObject pluginState;
    pluginState.insert("script", ui->te_pythonScript->toPlainText());
    return pluginState;
}

bool PythonRunner::setPluginStateInUi(const QJsonObject &pluginState)
{
    if (!canRecallPluginState(pluginState)) {
        return false;
    }

    ui->te_pythonScript->setPlainText(pluginState.value("script").toString());
    return true;
}

bool PythonRunner::canRecallPluginState(const QJsonObject &pluginState)
{
    // if pluginState does not have required fields, return false
    if (pluginState.isEmpty() == true) {
        return false;
    }
    if (!(pluginState.contains("script") && pluginState.value("script").isString())) {
        return false;
    }

    return true;
}

int PythonRunner::getMinInputContainers()
{
    return 1;
}

int PythonRunner::getMaxInputContainers()
{
    return 1;
}

QSharedPointer<const OperatorResult> PythonRunner::operateOnContainers(
        QList<QSharedPointer<const BitContainer>> inputContainers,
        const QJsonObject &recallablePluginState,
        QSharedPointer<ActionProgress> progressTracker)
{

    QMetaObject::invokeMethod(this, "clearOutputText", Qt::QueuedConnection);
    QSharedPointer<const OperatorResult> nullResult;
    if (inputContainers.length() != 1) {
        return nullResult;
    }
    if (!canRecallPluginState(recallablePluginState)) {
        return nullResult;
    }

    QString pythonPath = SettingsManager::getInstance().getPrivateSetting(PYTHON_PATH_KEY).toString();
    if (pythonPath.isEmpty()) {
        return nullResult;
    }

    QTemporaryDir dir;
    if (!dir.isValid()) {
        return nullResult;
    }

    QFile inputBitFile(dir.filePath("input_bit_container.json"));
    QFile outputBitFile(dir.filePath("output_bit_container.json"));
    if (!inputBitFile.open(QIODevice::Truncate | QIODevice::WriteOnly)) {
        return nullResult;
    }
    inputBitFile.write(inputContainers.at(0)->serializeJson().toJson());
    inputBitFile.close();

    QString coreScriptName = dir.filePath("core_script.py");
    QFile::copy(":/pythonrunner/scripts/runner.py", coreScriptName);
    QFile coreScriptFile(coreScriptName);
    coreScriptFile.setPermissions(QFile::ReadOwner | QFile::ExeOwner | QFile::WriteOwner);

    QFile userScriptFile(dir.filePath("user_script.py"));
    if (!userScriptFile.open(QIODevice::Truncate | QIODevice::WriteOnly)) {
        return nullResult;
    }
    userScriptFile.write(recallablePluginState.value("script").toString().toLatin1());
    userScriptFile.close();

    QFile errorFile(dir.filePath("error.log"));
    QFile stdoutFile(dir.filePath("stdout.log"));

    QStringList args = {coreScriptFile.fileName(), inputBitFile.fileName(), outputBitFile.fileName()};
    QProcess pythonProcess;
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QProcessEnvironment envUpdate;
    envUpdate.insert("PATH", env.value("PATH"));
    pythonProcess.setProcessEnvironment(envUpdate);
    pythonProcess.setWorkingDirectory(dir.path());
    pythonProcess.setStandardErrorFile(errorFile.fileName());
    pythonProcess.setStandardOutputFile(stdoutFile.fileName());
    pythonProcess.start(pythonPath, args);

    bool hasCancelled = false;
    while (!pythonProcess.waitForFinished(250)) {
        if (progressTracker->getCancelled() && !hasCancelled) {
            QFile abortFile(dir.filePath("abort"));
            abortFile.open(QIODevice::WriteOnly);
            abortFile.close();
            hasCancelled = true;
        }
        QFile progressFile(dir.filePath("progress"));
        if (progressFile.exists()) {
            if (progressFile.open(QIODevice::ReadOnly)) {
                QJsonDocument progressData = QJsonDocument::fromJson((progressFile.readAll()));
                QJsonObject progressJson = progressData.object();
                if (progressJson.contains("progress") && progressJson.value("progress").isDouble()) {
                    int progress = int(progressJson.value("progress").toDouble());
                    progressTracker->setProgressPercent(progress);
                }
            }
        }
    }

    QSharedPointer<OperatorResult> result(new OperatorResult());
    QJsonObject pluginState = recallablePluginState;

    if (errorFile.open(QIODevice::ReadOnly)) {
        QString output = errorFile.readAll();
        if (!output.isEmpty()) {
            QMetaObject::invokeMethod(
                    this,
                    "updateOutputText",
                    Qt::QueuedConnection,
                    Q_ARG(QString, "Python stderr:\n" + output + "\n\n"));
        }
        errorFile.close();
    }

    if (stdoutFile.open(QIODevice::ReadOnly)) {
        QString output = stdoutFile.readAll();
        if (!output.isEmpty()) {
            QMetaObject::invokeMethod(
                    this,
                    "updateOutputText",
                    Qt::QueuedConnection,
                    Q_ARG(QString, "Python stdout:\n" + output + "\n\n"));
        }
        stdoutFile.close();
    }

    if (!outputBitFile.open(QIODevice::ReadOnly)) {
        return nullResult;
    }
    QJsonDocument outputData = QJsonDocument::fromJson((outputBitFile.readAll()));
    QSharedPointer<BitContainer> outputContainer = QSharedPointer<BitContainer>(new BitContainer());

    outputContainer->deserializeJson(outputData);
    outputBitFile.close();

    result->setPluginState(pluginState);
    result->setOutputContainers({outputContainer});
    return std::move(result);
}

OperatorInterface* PythonRunner::createDefaultOperator()
{
    return new PythonRunner();
}

void PythonRunner::updateOutputText(QString text)
{
    ui->te_pluginOutput->appendPlainText(text);
    ui->te_pluginOutput->show();
}

void PythonRunner::clearOutputText()
{
    ui->te_pluginOutput->hide();
    ui->te_pluginOutput->clear();
}
