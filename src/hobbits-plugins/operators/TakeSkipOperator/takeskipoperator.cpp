#include "bitop.h"
#include "takeskipoperator.h"
#include "ui_takeskipoperator.h"

#include <QMessageBox>
#include <QToolTip>

TakeSkipOperator::TakeSkipOperator() :
    ui(new Ui::TakeSkipOperator()),
    m_stateHelper(new PluginStateHelper())
{
    m_stateHelper->addLineEditStringParameter("take_skip_string", [this](){return ui->le_takeSkip;});
    m_stateHelper->addCheckBoxBoolParameter("interleaved", [this](){return ui->cb_interleaved;}, true);
    m_stateHelper->addCheckBoxBoolParameter("frame_based", [this](){return ui->cb_frameBased;}, true);
    m_stateHelper->addSpinBoxIntParameter("deinterleave_channels", [this](){return ui->sb_deinterleave;}, true);
}

void TakeSkipOperator::interleaveSelectionChanged()
{
    ui->cb_deinterleaved->setEnabled(!ui->cb_interleaved->isChecked());
    ui->cb_interleaved->setEnabled(!ui->cb_deinterleaved->isChecked());
    ui->sb_deinterleave->setVisible(ui->cb_deinterleaved->isChecked());

    if (ui->cb_deinterleaved->isChecked()) {
        ui->sb_deinterleave->setRange(2, 100);
        ui->sb_deinterleave->setValue(2);
    }
    else {
        ui->sb_deinterleave->setRange(1, 1);
        ui->sb_deinterleave->setValue(1);
    }
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
    return m_stateHelper->getPluginStateFromUi();
}

bool TakeSkipOperator::setPluginStateInUi(const QJsonObject &pluginState)
{
    return m_stateHelper->applyPluginStateToUi(pluginState);
}

bool TakeSkipOperator::canRecallPluginState(const QJsonObject &pluginState)
{
    return m_stateHelper->validatePluginState(pluginState);
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

    int outputCount = 1;
    if (recallablePluginState.contains("deinterleave_channels") && recallablePluginState.value("deinterleave_channels").toInt() > 1) {
        outputCount = recallablePluginState.value("deinterleave_channels").toInt();
    }
    QList<OutputHandle> outputs;
    QHash<QSharedPointer<BitArray>, QVector<Range>> outputFrames;
    for (int i = 0; i < outputCount; i++) {
        OutputHandle handle;
        handle.idx = 0;
        handle.bits = QSharedPointer<BitArray>(new BitArray(outputBufferSize));
        outputs.append(handle);
    }

    int lastPercent = 0;

    for (int currFrame = 0; currFrame < frameCount; currFrame++) {
        qint64 bitsProcessed = 0;
        bool reachedEnd = false;
        for (int i = 0; i < inputs.size(); i++) {
            inputs[i].second = 0;
        }

        while (true) {
            for (int i = 0; i < inputs.size(); i++) {
                for (int k = 0; k < outputs.size(); k++) {
                    auto outputBits = outputs.at(k).bits;
                    qint64 outputIdx = outputs.at(k).idx;
                    qint64 outStart = outputIdx;

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

                    if (outputIdx > outStart) {
                        if (frameBased) {
                            outputs[k].frames.append(Range(outStart, outputIdx - 1));
                        }
                        outputs[k].idx = outputIdx;
                    }

                    bitsProcessed += inputIdx - start;
                    inputs[i].second = inputIdx;
                }
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
    }

    QList<QSharedPointer<BitContainer>> outputContainers;
    int outputNum = 0;
    for (auto output: outputs) {
        output.bits->resize(output.idx);
        auto outputContainer = QSharedPointer<BitContainer>(new BitContainer());
        outputContainers.append(outputContainer);
        outputContainer->setBits(output.bits);
        if (frameBased) {
            outputContainer->bitInfo()->setFrames(output.frames);
        }
        if (inputContainers.size() > 1) {
            outputContainer->setName(QString("%1 Interleave").arg(recallablePluginState.value("take_skip_string").toString()));
        }
        else if (outputs.size() > 1) {
            outputContainer->setName(QString("%1 Deinterleave[%2] <- %3")
                                     .arg(recallablePluginState.value("take_skip_string").toString())
                                     .arg(outputNum)
                                     .arg(inputContainers.at(0)->name()));
        }
        else {
            outputContainer->setName(QString("%1 <- %2")
                                    .arg(recallablePluginState.value("take_skip_string").toString())
                                    .arg(inputContainers.at(0)->name()));
        }
        outputNum++;
    }
    return OperatorResult::result(outputContainers, recallablePluginState);
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

    connect(ui->cb_interleaved, SIGNAL(toggled(bool)), this, SLOT(interleaveSelectionChanged()));
    connect(ui->cb_deinterleaved, SIGNAL(toggled(bool)), this, SLOT(interleaveSelectionChanged()));
    interleaveSelectionChanged();
}

void TakeSkipOperator::showHelp()
{
    QMessageBox msg;
    msg.setText("Take Skip Commands");
    QString text = "t - take\ns - skip\nr - take in reverse\ni - invert\no - append a 1\nz - append a 0";
    text += "\n\n Uppercase letters can be used to indicate Bytes instead of bits (e.g. 'T10S4')";
    text += "\nThe character '*' can be used instead of a number to indicate 'all remaining bits'";
    text += "\nInterleave: apply the operation in a round-robin fashion between multiple input containers.";
    text += "\nFrame-based: apply the operation to each frame of the container independently";
    msg.setInformativeText(text);
    msg.setDefaultButton(QMessageBox::Cancel);
    msg.exec();
}
