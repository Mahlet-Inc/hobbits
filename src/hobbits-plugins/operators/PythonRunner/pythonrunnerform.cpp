#include "pythonrunnerform.h"
#include <QMessageBox>
#include "ui_pythonrunnerform.h"
#include "pythonsyntaxhighlighter.h"

PythonRunnerForm::PythonRunnerForm(QSharedPointer<ParameterDelegate> delegate):
    ui(new Ui::PythonRunnerForm()),
    m_paramHelper(new ParameterHelper(delegate))
{
    ui->setupUi(this);
    ui->te_pluginOutput->hide();
    connect(ui->pb_scriptHelp, SIGNAL(pressed()), this, SLOT(openHelpDialog()));

    new PythonSyntaxHighlighter(ui->te_pythonScript->document());

    m_paramHelper->addTextEditStringParameter("script", ui->te_pythonScript);
}

PythonRunnerForm::~PythonRunnerForm()
{
    delete ui;
}

QString PythonRunnerForm::title()
{
    return "Write Python Script";
}

bool PythonRunnerForm::setParameters(QJsonObject parameters)
{
    return m_paramHelper->applyParametersToUi(parameters);
}

QJsonObject PythonRunnerForm::parameters()
{
    return m_paramHelper->getParametersFromUi();
}

void PythonRunnerForm::openHelpDialog()
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

void PythonRunnerForm::updateOutputText(QString text)
{
    ui->te_pluginOutput->appendPlainText(text);
    ui->te_pluginOutput->show();
}

void PythonRunnerForm::clearOutputText()
{
    ui->te_pluginOutput->hide();
    ui->te_pluginOutput->clear();
}
