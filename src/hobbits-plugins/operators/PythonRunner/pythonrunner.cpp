#include "pythonrunner.h"
#include "ui_pythonrunner.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QMetaObject>
#include <QObject>
#include <QProcess>
#include <QProcessEnvironment>
#include <QTemporaryDir>
#include <QStandardPaths>

#include "pythonsyntaxhighlighter.h"
#include "embeddedpythoninterpreter.h"

#include "settingsmanager.h"

const QString PYTHON_PATH_KEY = "python_runner_path";

PythonRunner::PythonRunner() :
    ui(new Ui::PythonRunner()),
    m_stateHelper(new PluginStateHelper())
{
    m_stateHelper->addTextEditStringParameter("script", [this](){return ui->te_pythonScript;});
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

    // Auto-locate if empty
    if (ui->le_pythonPath->text().isEmpty()) {
        QString pythonPath = QStandardPaths::findExecutable("python3");
        if (!pythonPath.isEmpty()) {
            ui->le_pythonPath->setText(pythonPath);
            SettingsManager::getInstance().setPrivateSetting(PYTHON_PATH_KEY, pythonPath);
        }
        else {
            pythonPath = QStandardPaths::findExecutable("python");
            if (!pythonPath.isEmpty()) {
                ui->le_pythonPath->setText(pythonPath);
                SettingsManager::getInstance().setPrivateSetting(PYTHON_PATH_KEY, pythonPath);
            }
        }
    }

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
    return m_stateHelper->getPluginStateFromUi();
}

bool PythonRunner::setPluginStateInUi(const QJsonObject &pluginState)
{
    return m_stateHelper->applyPluginStateToUi(pluginState);
}

bool PythonRunner::canRecallPluginState(const QJsonObject &pluginState)
{
    return m_stateHelper->validatePluginState(pluginState);
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

    QFile userScriptFile(dir.filePath("user_script.py"));
    if (!userScriptFile.open(QIODevice::Truncate | QIODevice::WriteOnly)) {
        return nullResult;
    }
    userScriptFile.write(recallablePluginState.value("script").toString().toLatin1());
    userScriptFile.close();

    auto interpreter = QSharedPointer<EmbeddedPythonInterpreter>(new EmbeddedPythonInterpreter());

    auto outputBits = QSharedPointer<BitArray>(new BitArray());
    interpreter->runScript(userScriptFile.fileName(), inputContainers.at(0)->bits(), outputBits);

    QString output = interpreter->readErrors();
    if (!output.isEmpty()) {
        QMetaObject::invokeMethod(
                this,
                "updateOutputText",
                Qt::QueuedConnection,
                Q_ARG(QString, "Python stderr:\n" + output + "\n\n"));
        if (outputBits->sizeInBits() < 1) {
            return OperatorResult::error("Errors encountered during execution");
        }
    }

    QSharedPointer<BitContainer> outputContainer = QSharedPointer<BitContainer>(new BitContainer());
    outputContainer->setBits(outputBits);
    outputContainer->setName(QString("python <- %1").arg(inputContainers.at(0)->name()));

    return OperatorResult::result({outputContainer}, recallablePluginState);
}

void PythonRunner::previewBits(QSharedPointer<BitContainerPreview> container)
{
    Q_UNUSED(container)
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
