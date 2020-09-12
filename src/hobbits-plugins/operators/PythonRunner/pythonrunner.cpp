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

#include "settingsmanager.h"
#include "hobbitspython.h"

PythonRunner::PythonRunner() :
    ui(new Ui::PythonRunner()),
    m_stateHelper(new PluginStateHelper()),
    m_hasUi(false)
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
    m_hasUi = true;

    ui->te_pluginOutput->hide();

    connect(ui->pb_scriptHelp, SIGNAL(pressed()), this, SLOT(openHelpDialog()));

    new PythonSyntaxHighlighter(ui->te_pythonScript->document());
}

void PythonRunner::openHelpDialog()
{
    QMessageBox msg;
    msg.setWindowTitle("Python Runner API");
    QString docs;
    docs += "BitArray.<strong>size</strong>()<br/>";
    docs += "The length of the container in bits<br/><br/>";
    docs += "BitArray.<strong>at</strong>(<em>position</em>)<br/>";
    docs += "Gets the bit value at the given position<br/><br/>";
    docs += "BitArray.<strong>set</strong>(<em>position</em>, <em>value</em>)<br/>";
    docs += "Sets the bit value at the given position<br/><br/>";
    docs += "BitArray.<strong>resize</strong>(<em>length</em>)<br/>";
    docs += "Sets the container's length in bits<br/><br/>";
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
    if (inputContainers.length() != 1) {
        return OperatorResult::error("Requires a single input bit container");
    }
    if (!canRecallPluginState(recallablePluginState)) {
        return OperatorResult::error("Invalid plugin state");
    }

    QTemporaryDir dir;
    if (!dir.isValid()) {
        return OperatorResult::error("Could not create temporary directory");
    }

    QFile userScriptFile(dir.filePath("user_script.py"));
    if (!userScriptFile.open(QIODevice::Truncate | QIODevice::WriteOnly)) {
        return OperatorResult::error("Could not write script to temporary directory");
    }
    userScriptFile.write(recallablePluginState.value("script").toString().toLatin1());
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
    if (!result->getStdOut().isEmpty()) {
        output += "Python stdout:\n" + result->getStdOut() + "\n\n";
    }
    if (!result->getStdErr().isEmpty()) {
        output += "Python stderr:\n" + result->getStdErr() + "\n\n";
    }
    if (!result->errors().isEmpty()) {
        output += "Other errors:\n" + result->errors().join("\n") + "\n\n";
    }
    if (!output.isEmpty()) {
        QMetaObject::invokeMethod(
                this,
                "updateOutputText",
                Qt::QueuedConnection,
                Q_ARG(QString, output));
    }

    QSharedPointer<BitContainer> outputContainer = BitContainer::create(outputBits);
    outputContainer->setInfo(outputInfo);
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
    if (m_hasUi) {
        ui->te_pluginOutput->appendPlainText(text);
        ui->te_pluginOutput->show();
    }
}

void PythonRunner::clearOutputText()
{
    if (m_hasUi) {
        ui->te_pluginOutput->hide();
        ui->te_pluginOutput->clear();
    }
}
