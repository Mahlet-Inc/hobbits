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
    pluginState.insert("interleaved", ui->cb_interleaved->checkState() == Qt::Checked);
    pluginState.insert("frame_based", ui->cb_frameBased->checkState() == Qt::Checked);
    return pluginState;
}

bool TakeSkipOperator::setPluginStateInUi(const QJsonObject &pluginState)
{
    if (!canRecallPluginState(pluginState)) {
        return false;
    }

    ui->le_takeSkip->setText(pluginState.value("take_skip_string").toString());
    if (pluginState.contains("interleaved")) {
        if (pluginState.value("interleaved").toBool()) {
            ui->cb_interleaved->setChecked(true);
        }
        else {
            ui->cb_interleaved->setChecked(false);
        }
    }
    if (pluginState.contains("frame_based")) {
        if (pluginState.value("frame_based").toBool()) {
            ui->cb_frameBased->setChecked(true);
        }
        else {
            ui->cb_frameBased->setChecked(false);
        }
    }
    return true;
}

bool TakeSkipOperator::canRecallPluginState(const QJsonObject &pluginState)
{
    return pluginState.contains("take_skip_string") && pluginState.value("take_skip_string").isString();
}

int TakeSkipOperator::getMinInputContainers(const QJsonObject &pluginState)
{
    if (pluginState.contains("interleaved") && pluginState.value("interleaved").toBool()) {
        return 2;
    }
    return 1;
}

int TakeSkipOperator::getMaxInputContainers(const QJsonObject &pluginState)
{
    if (pluginState.contains("interleaved") && pluginState.value("interleaved").toBool()) {
        return 100;
    }
    return 1;
}

QSharedPointer<const OperatorResult> TakeSkipOperator::operateOnContainers(
        QList<QSharedPointer<const BitContainer>> inputContainers,
        const QJsonObject &recallablePluginState,
        QSharedPointer<ActionProgress> progressTracker)
{
    QSharedPointer<const OperatorResult> nullResult;

    if (inputContainers.size() < getMinInputContainers(recallablePluginState)
        || inputContainers.size() > getMaxInputContainers(recallablePluginState)) {
        return nullResult;
    }

    QList<QSharedPointer<BitOp>> ops = BitOp::parseOps(recallablePluginState.value("take_skip_string").toString());
    if (ops.isEmpty()) {
        return nullResult;
    }

    bool frameBased =
        (recallablePluginState.contains("frame_based") && recallablePluginState.value("frame_based").toBool());
    int frameCount = inputContainers.at(0)->frames().size();
    QList<QPair<QVector<Frame>, qint64>> inputs;
    for (auto inputContainer : inputContainers) {
        if (frameBased) {
            frameCount = qMin(frameCount, inputContainer->frames().size());
            inputs.append({inputContainer->frames(), 0});
        }
        else {
            frameCount = 1;
            inputs.append({{Frame(inputContainer->bits(), 0,
                    inputContainer->bits()->sizeInBits() - 1)}, 0});
        }
    }

    qint64 inputBitCount = 0;
    qint64 inputStepTotal = 0;
    qint64 outputStepTotal = 0;
    for (QSharedPointer<BitOp> op : ops) {
        for (auto input : inputs) {
            for (int i = 0; i < frameCount; i++) {
                Frame frame = input.first[i];
                // need to check for integer overflow
                inputBitCount += frame.size();
                qint64 frameStep = op->inputStep(frame.size());
                qint64 inTotal = frameStep + inputStepTotal;
                if (inTotal < frameStep || inTotal < inputStepTotal) {
                    inputStepTotal = LLONG_MAX;
                    break;
                }
                else {
                    inputStepTotal = inTotal;
                }
                frameStep = op->outputStep(frame.size());
                qint64 outTotal = frameStep + outputStepTotal;
                if (outTotal < frameStep || outTotal < outputStepTotal) {
                    outputStepTotal = LLONG_MAX;
                    break;
                }
                else {
                    outputStepTotal = outTotal;
                }
            }
        }
    }
    if (inputStepTotal < 1) {
        return nullResult;
    }

    qint64 opCycles = inputBitCount / inputStepTotal
                      + (inputBitCount % inputStepTotal ? 1 : 0);
    qint64 outputBufferSize = opCycles * outputStepTotal;
    if (outputBufferSize < outputStepTotal) {
        outputBufferSize = LLONG_MAX;
    }

    QSharedPointer<BitArray> outputBits = QSharedPointer<BitArray>(new BitArray(outputBufferSize));

    qint64 outputIdx = 0;
    int lastPercent = 0;
    QVector<Range> outputFrames;

    for (int currFrame = 0; currFrame < frameCount; currFrame++) {
        qint64 bitsProcessed = 0;
        bool reachedEnd = false;
        for (int i = 0; i < inputs.size(); i++) {
            inputs[i].second = 0;
        }

        qint64 frameStart = outputIdx;

        while (true) {
            for (int i = 0; i < inputs.size(); i++) {
                Frame inputBits = inputs.at(i).first.at(currFrame);
                qint64 inputIdx = inputs.at(i).second;
                qint64 start = inputIdx;
                for (QSharedPointer<BitOp> op : ops) {
                    op->apply(inputBits, outputBits, inputIdx, outputIdx);
                }
                if (inputIdx >= inputBits.size()) {
                    // We've reached the end of this input, so we should finish
                    reachedEnd = true;
                }

                bitsProcessed += inputIdx - start;
                inputs[i].second = inputIdx;
            }
            if (reachedEnd) {
                break;
            }

            if (bitsProcessed > 0) {
                int nextPercent = int(double(bitsProcessed) / double(inputBitCount) * 100.0);
                if (nextPercent > lastPercent) {
                    lastPercent = nextPercent;
                    progressTracker->setProgressPercent(nextPercent);
                }
            }

            if (progressTracker->getCancelled()) {
                auto cancelledPair = QPair<QString, QJsonValue>("error", QJsonValue("Processing cancelled"));
                auto cancelled = (new OperatorResult())->setPluginState(QJsonObject({cancelledPair}));
                return QSharedPointer<const OperatorResult>(cancelled);
            }
        }
        outputFrames.append(Range(frameStart, outputIdx - 1));
    }

    outputBits->resize(outputIdx);
    QSharedPointer<BitContainer> bitContainer = QSharedPointer<BitContainer>(new BitContainer());
    bitContainer->setBits(outputBits);
    if (frameBased) {
        bitContainer->bitInfo()->setFrames(outputFrames);
    }
    if (inputContainers.size() > 1) {
        bitContainer->setName(QString("%1 Interleave").arg(recallablePluginState.value("take_skip_string").toString()));
    }
    else {
        bitContainer->setName(QString("%1 <- %2")
                                .arg(recallablePluginState.value("take_skip_string").toString())
                                .arg(inputContainers.at(0)->name()));
    }
    return OperatorResult::result({ bitContainer }, recallablePluginState);
}

void TakeSkipOperator::previewBits(QSharedPointer<BitContainerPreview> container)
{
    Q_UNUSED(container)
}

OperatorInterface* TakeSkipOperator::createDefaultOperator()
{
    return new TakeSkipOperator();
}

void TakeSkipOperator::requestRun()
{
    if (!m_pluginCallback.isNull()) {
        m_pluginCallback->requestOperatorRun(getName());
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
    QString text = "t - take\ns - skip\nr - take in reverse\ni - invert\no - append a 1\nz - append a 0";
    text += "\n\nThe character '*' can be used instead of a number to indicate 'all remaining bits'";
    text += "\nInterleave: apply the operation in a round-robin fashion between multiple input containers.";
    text += "\nFrame-based: apply the operation to each frame of the container independently";
    msg.setInformativeText(text);
    msg.setDefaultButton(QMessageBox::Cancel);
    msg.exec();
}
