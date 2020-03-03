#include "bitop.h"
#include "takeskipoperator.h"
#include "ui_takeskipoperator.h"

#include <QMessageBox>
#include <QToolTip>

TakeSkipOperator::TakeSkipOperator() :
    ui(new Ui::TakeSkipOperator())
{
}

QString TakeSkipOperator::getName()
{
    return "Take Skip";
}

void TakeSkipOperator::provideCallback(QSharedPointer<PluginCallback> pluginCallback)
{
    // the plugin callback allows the self-triggering of operateOnContainers
    m_pluginCallback = pluginCallback;
}

QJsonObject TakeSkipOperator::getStateFromUi()
{
    QJsonObject pluginState;
    pluginState.insert("take_skip_string", ui->le_takeSkip->text());
    return pluginState;
}

bool TakeSkipOperator::setPluginStateInUi(const QJsonObject &pluginState)
{
    if (!canRecallPluginState(pluginState)) {
        return false;
    }

    ui->le_takeSkip->setText(pluginState.value("take_skip_string").toString());
    return true;
}

bool TakeSkipOperator::canRecallPluginState(const QJsonObject &pluginState)
{
    return pluginState.contains("take_skip_string") && pluginState.value("take_skip_string").isString();
}

int TakeSkipOperator::getMinInputContainers()
{
    return 1;
}

int TakeSkipOperator::getMaxInputContainers()
{
    return 1;
}

QSharedPointer<const OperatorResult> TakeSkipOperator::operateOnContainers(
        QList<QSharedPointer<const BitContainer>> inputContainers,
        const QJsonObject &recallablePluginState,
        QSharedPointer<ActionProgress> progressTracker)
{
    QList<QSharedPointer<BitContainer>> outputContainers;
    QSharedPointer<const OperatorResult> nullResult;

    if (inputContainers.size() != 1) {
        return nullResult;
    }

    QList<QSharedPointer<BitOp>> ops = BitOp::parseOps(recallablePluginState.value("take_skip_string").toString());
    if (ops.isEmpty()) {
        return nullResult;
    }

    QSharedPointer<const BitArray> inputBits = inputContainers.at(0)->getBaseBits();

    qint64 inputStepTotal = 0;
    qint64 outputStepTotal = 0;
    for (QSharedPointer<BitOp> op : ops) {
        // need to check for integer overflow
        qint64 inTotal = op->inputStep(inputBits->sizeInBits()) + inputStepTotal;
        if (inTotal < op->inputStep(inputBits->sizeInBits()) || inTotal < inputStepTotal) {
            inputStepTotal = LLONG_MAX;
        }
        else {
            inputStepTotal = inTotal;
        }
        qint64 outTotal = op->outputStep(inputBits->sizeInBits()) + outputStepTotal;
        if (outTotal < op->outputStep(inputBits->sizeInBits()) || outTotal < outputStepTotal) {
            outputStepTotal = LLONG_MAX;
        }
        else {
            outputStepTotal = outTotal;
        }
    }
    if (inputStepTotal < 1) {
        return nullResult;
    }

    qint64 opCycles = inputContainers.at(0)->getBaseBits()->sizeInBits() / inputStepTotal
                   + (inputContainers.at(0)->getBaseBits()->sizeInBits() % inputStepTotal ? 1 : 0);
    qint64 outputBufferSize = opCycles * outputStepTotal;
    if (outputBufferSize < outputStepTotal) {
        outputBufferSize = LLONG_MAX;
    }

    qint64 inputIdx = 0;
    qint64 outputIdx = 0;

    QSharedPointer<BitArray> outputBits = QSharedPointer<BitArray>(new BitArray(outputBufferSize));

    int lastPercent = 0;
    while (inputIdx < inputBits->sizeInBits()) {
        for (QSharedPointer<BitOp> op : ops) {
            op->apply(inputBits, outputBits, inputIdx, outputIdx);
        }
        if (inputIdx > 0) {
            int nextPercent = int(double(inputIdx) / double(inputBits->sizeInBits()) * 100.0);
            if (nextPercent > lastPercent) {
                lastPercent = nextPercent;
                progressTracker->setProgressPercent(nextPercent);
            }
        }
        if (progressTracker->getCancelled()) {
            return QSharedPointer<const OperatorResult>(
                    (new OperatorResult())->setPluginState(
                            QJsonObject(
                                    {QPair<QString, QJsonValue>(
                                            "error",
                                            QJsonValue("Processing cancelled"))}))
                    );
        }
    }

    outputBits->resize(outputIdx);
    QSharedPointer<BitContainer> bitContainer = QSharedPointer<BitContainer>(new BitContainer());
    bitContainer->setBytes(outputBits);
    outputContainers.append(bitContainer);
    QJsonObject pluginState(recallablePluginState);
    pluginState.insert(
            "container_name",
            QString("%1 <- %2").arg(recallablePluginState.value("take_skip_string").toString()).arg(
                    inputContainers.at(0)
                    ->
                    getName()));
    return QSharedPointer<const OperatorResult>(
            (new OperatorResult())->setOutputContainers(
                    outputContainers)->setPluginState(pluginState));
}

OperatorInterface* TakeSkipOperator::createDefaultOperator()
{
    return new TakeSkipOperator();
}

void TakeSkipOperator::requestRun()
{
    if (!m_pluginCallback.isNull()) {
        m_pluginCallback->requestRun(getName());
    }
}

void TakeSkipOperator::applyToWidget(QWidget *widget)
{
    ui->setupUi(widget);
    connect(ui->btnInfo, SIGNAL(clicked()), this, SLOT(showHelp()));
    connect(ui->le_takeSkip, SIGNAL(returnPressed()), this, SLOT(requestRun()));
}

void TakeSkipOperator::showHelp()
{
    QMessageBox msg;
    msg.setText("TakeSkip Commands");
    msg.setInformativeText("t - take\ns - skip\nr - take in reverse\ni - invert\no - append a 1\nz - append a 0");
    msg.setDefaultButton(QMessageBox::Cancel);
    msg.exec();
}
