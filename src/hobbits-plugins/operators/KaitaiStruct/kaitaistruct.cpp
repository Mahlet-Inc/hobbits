#include "kaitaistruct.h"
#include "ui_kaitaistruct.h"
#include <QObject>
#include "settingsmanager.h"
#include <QStandardPaths>
#include <QFileDialog>
#include <QTemporaryDir>
#include <QProcess>
#include <QProcessEnvironment>
#include <QJsonArray>

const QString PYTHON_PATH_KEY = "python_runner_path";
const QString KAITAI_PATH_KEY = "kaitai_struct_compiler_path";

KaitaiStruct::KaitaiStruct() :
    ui(new Ui::KaitaiStruct())
{

}

OperatorInterface* KaitaiStruct::createDefaultOperator()
{
    return new KaitaiStruct();
}

//Return name of operator
QString KaitaiStruct::getName()
{
    return "Kaitai Struct";
}

void KaitaiStruct::provideCallback(QSharedPointer<PluginCallback> pluginCallback)
{
    // the plugin callback allows the self-triggering of operateOnContainers
    m_pluginCallback = pluginCallback;
}

void KaitaiStruct::applyToWidget(QWidget *widget)
{
    ui->setupUi(widget);

    connect(ui->tb_choosePython, SIGNAL(pressed()), this, SLOT(openPythonPathDialog()));
    connect(ui->tb_chooseKsc, SIGNAL(pressed()), this, SLOT(openKscPathDialog()));
    ui->le_python->setText(SettingsManager::getInstance().getPrivateSetting(PYTHON_PATH_KEY).toString());
    ui->le_ksc->setText(SettingsManager::getInstance().getPrivateSetting(KAITAI_PATH_KEY).toString());

    // Auto-locate if empty
    if (ui->le_python->text().isEmpty()) {
        QString pythonPath = QStandardPaths::findExecutable("python3");
        if (!pythonPath.isEmpty()) {
            ui->le_python->setText(pythonPath);
            SettingsManager::getInstance().setPrivateSetting(PYTHON_PATH_KEY, pythonPath);
        }
        else {
            pythonPath = QStandardPaths::findExecutable("python");
            if (!pythonPath.isEmpty()) {
                ui->le_python->setText(pythonPath);
                SettingsManager::getInstance().setPrivateSetting(PYTHON_PATH_KEY, pythonPath);
            }
        }
    }
}

void KaitaiStruct::openPythonPathDialog()
{
    QString fileName = QFileDialog::getOpenFileName(
            nullptr,
            tr("Select Python"),
            SettingsManager::getInstance().getPrivateSetting(PYTHON_PATH_KEY).toString(),
            tr("Python (python*)"));
    if (fileName.isEmpty()) {
        return;
    }
    ui->le_python->setText(fileName);
    SettingsManager::getInstance().setPrivateSetting(PYTHON_PATH_KEY, fileName);
}

void KaitaiStruct::openKscPathDialog()
{
    QString fileName = QFileDialog::getOpenFileName(
            nullptr,
            tr("Select Kaitai Struct Compiler"),
            SettingsManager::getInstance().getPrivateSetting(KAITAI_PATH_KEY).toString(),
            tr("KSC (ksc*, kaitai-struct-compiler*)"));
    if (fileName.isEmpty()) {
        return;
    }
    ui->le_ksc->setText(fileName);
    SettingsManager::getInstance().setPrivateSetting(KAITAI_PATH_KEY, fileName);
}

bool KaitaiStruct::canRecallPluginState(const QJsonObject &pluginState)
{
    //if pluginState does not have required fields, return false
    if(pluginState.isEmpty()==true){
        return false;
    }
    if (!(pluginState.contains("katai_struct_yaml") && pluginState.value("katai_struct_yaml").isString())) {
        return false;
    }

    return true;
}

bool KaitaiStruct::setPluginStateInUi(const QJsonObject &pluginState)
{
    if (!canRecallPluginState(pluginState)) {
        return false;
    }

    // Set the UI fields based on the plugin state
    ui->te_ksy->setPlainText(pluginState.value("katai_struct_yaml").toString());

    return true;
}

QJsonObject KaitaiStruct::getStateFromUi()
{
    QJsonObject pluginState;
    pluginState.insert("katai_struct_yaml", ui->te_ksy->toPlainText());
    return pluginState;
}

int KaitaiStruct::getMinInputContainers(const QJsonObject &pluginState)
{
    return 1;
}

int KaitaiStruct::getMaxInputContainers(const QJsonObject &pluginState)
{
    return 1;
}

QSharedPointer<const OperatorResult> KaitaiStruct::operateOnContainers(
        QList<QSharedPointer<const BitContainer> > inputContainers,
        const QJsonObject &recallablePluginState,
        QSharedPointer<ActionProgress> progressTracker)
{
    QSharedPointer<OperatorResult> result(new OperatorResult());
    //Perform bit operations here

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

    QString kscPath = SettingsManager::getInstance().getPrivateSetting(KAITAI_PATH_KEY).toString();
    if (kscPath.isEmpty()) {
        return nullResult;
    }

    QTemporaryDir dir;
    if (!dir.isValid()) {
        return nullResult;
    }

    QFile inputBitFile(dir.filePath("input_bit_container.bits"));
    QFile outputRangeFile(dir.filePath("parsed_ranges.json"));
    if (!inputBitFile.open(QIODevice::Truncate | QIODevice::WriteOnly)) {
        return nullResult;
    }
    inputContainers.at(0)->getBaseBits()->writeTo(&inputBitFile);
    inputBitFile.close();

    QString coreScriptName = dir.filePath("core_script.py");
    QFile::copy(":/kaitaistruct/scripts/runner.py", coreScriptName);
    QFile coreScriptFile(coreScriptName);
    coreScriptFile.setPermissions(QFile::ReadOwner | QFile::ExeOwner | QFile::WriteOwner);

    QFile errorFile(dir.filePath("error.log"));
    QFile stdoutFile(dir.filePath("stdout.log"));

    QFile ksy(dir.filePath("custom.ksy"));
    if (!ksy.open(QIODevice::Truncate | QIODevice::WriteOnly)) {
        return nullResult;
    }
    ksy.write(recallablePluginState.value("katai_struct_yaml").toString().toLocal8Bit());
    ksy.close();

    QStringList kscAgs = {"--debug", "-t", "python", ksy.fileName()};
    QProcess kscProcess;
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QProcessEnvironment envUpdate;
    envUpdate.insert("PATH", env.value("PATH"));
    kscProcess.setProcessEnvironment(envUpdate);
    kscProcess.setWorkingDirectory(dir.path());
    kscProcess.setStandardErrorFile(errorFile.fileName());
    kscProcess.setStandardOutputFile(stdoutFile.fileName());
    kscProcess.start(kscPath, kscAgs);

    kscProcess.waitForFinished();

    if (errorFile.open(QIODevice::ReadOnly)) {
        QString output = errorFile.readAll();
        if (!output.isEmpty()) {
            QMetaObject::invokeMethod(
                    this,
                    "updateOutputText",
                    Qt::QueuedConnection,
                    Q_ARG(QString, "kaitai-stuct-compiler stderr:\n" + output + "\n\n"));
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
                    Q_ARG(QString, "kaitai-stuct-compiler stdout:\n" + output + "\n\n"));
        }
        stdoutFile.close();
    }

    QStringList pythonArgs = {coreScriptFile.fileName(), inputBitFile.fileName(), outputRangeFile.fileName()};
    QProcess pythonProcess;
    pythonProcess.setProcessEnvironment(envUpdate);
    pythonProcess.setWorkingDirectory(dir.path());
    pythonProcess.setStandardErrorFile(errorFile.fileName());
    pythonProcess.setStandardOutputFile(stdoutFile.fileName());
    pythonProcess.start(pythonPath, pythonArgs);

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

    QSharedPointer<BitContainer> outputContainer = QSharedPointer<BitContainer>(new BitContainer());
    outputContainer->setBytes(inputContainers.at(0)->getBaseBits());
    if (!outputRangeFile.open(QIODevice::ReadOnly)) {
        return nullResult;
    }
    QJsonDocument outputData = QJsonDocument::fromJson((outputRangeFile.readAll()));
    if (!outputData.isObject()) {
        return nullResult;
    }
    QJsonObject outputObj = outputData.object();
    if (!outputObj.contains("sections") || !outputObj.value("sections").isArray()) {
        return nullResult;
    }
    QList<QList<Range>> ranges = {{}, {}, {}, {}, {}};
    int range = 0;
    QJsonArray sections = outputObj.value("sections").toArray();
    for (auto section: sections) {
        if (!section.isObject()) {
            continue;
        }
        QJsonObject s = section.toObject();
        if (!s.contains("start") || !s.contains("end") || !s.value("start").isDouble() || !s.value("end").isDouble()) {
            continue;
        }
        ranges[range].append(Range(qint64(s.value("start").toDouble())*8, qint64(s.value("end").toDouble())*8 - 1));
        range = (range + 1) % 5;
    }
    outputRangeFile.close();
    outputContainer->setHighlights("highlight_1", ranges.at(0));
    outputContainer->setHighlights("highlight_2", ranges.at(1));
    outputContainer->setHighlights("highlight_3", ranges.at(2));
    outputContainer->setHighlights("highlight_4", ranges.at(3));
    outputContainer->setHighlights("highlight_5", ranges.at(4));

    result->setPluginState(recallablePluginState);
    result->setOutputContainers({outputContainer});
    return std::move(result);
}

void KaitaiStruct::updateOutputText(QString text)
{
    ui->te_output->appendPlainText(text);
}

void KaitaiStruct::clearOutputText()
{
    ui->te_output->clear();
}
