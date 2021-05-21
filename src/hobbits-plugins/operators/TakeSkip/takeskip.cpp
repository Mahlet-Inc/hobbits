#include "bitop.h"
#include "takeskip.h"
#include <QMessageBox>
#include <QToolTip>
#include "takeskipeditor.h"

struct OutputHandle {
    qint64 idx;
    QSharedPointer<BitArray> bits;
    QSharedPointer<RangeSequence> frames;
};

TakeSkip::TakeSkip()
{
    QList<ParameterDelegate::ParameterInfo> infos = {
        {"take_skip_string", ParameterDelegate::ParameterType::String, false},
        {"interleaved", ParameterDelegate::ParameterType::Boolean, true},
        {"frame_based", ParameterDelegate::ParameterType::Boolean, true},
        {"deinterleave_channels", ParameterDelegate::ParameterType::Integer, true}
    };

    m_delegate = ParameterDelegate::create(
                    infos,
                    [](const Parameters &parameters) {
                        QString base = parameters.value("take_skip_string").toString();

                        if (parameters.contains("interleaved") && parameters.value("interleaved").toBool()) {
                            return QString("%1 Interleave").arg(base);
                        }
                        else if (parameters.contains("deinterleave_channels") && parameters.value("deinterleave_channels").toDouble() > 1) {
                            return QString("%1 Deinterleave").arg(base);
                        }
                        else {
                            return base;
                        }
                    },
                    [](QSharedPointer<ParameterDelegate> delegate, QSize size) {
                        Q_UNUSED(size)
                        return new TakeSkipEditor(delegate);
                    });
}

OperatorInterface* TakeSkip::createDefaultOperator()
{
    return new TakeSkip();
}

QString TakeSkip::name()
{
    return "Take Skip";
}

QString TakeSkip::description()
{
    return "Bitwise data modifications with simple expression strings";
}

QStringList TakeSkip::tags()
{
    return {"Generic"};
}

QSharedPointer<ParameterDelegate> TakeSkip::parameterDelegate()
{
    return m_delegate;
}

int TakeSkip::getMinInputContainers(const Parameters &parameters)
{
    if (parameters.contains("interleaved") && parameters.value("interleaved").toBool()) {
        return 2;
    }
    return 1;
}

int TakeSkip::getMaxInputContainers(const Parameters &parameters)
{
    if (parameters.contains("interleaved") && parameters.value("interleaved").toBool()) {
        return 100;
    }
    return 1;
}

QSharedPointer<const OperatorResult> TakeSkip::operateOnBits(
        QList<QSharedPointer<const BitContainer>> inputContainers,
        const Parameters &parameters,
        QSharedPointer<PluginActionProgress> progress)
{
    QSharedPointer<const OperatorResult> nullResult;

    if (inputContainers.size() < getMinInputContainers(parameters)
        || inputContainers.size() > getMaxInputContainers(parameters)) {
        return nullResult;
    }

    QList<QSharedPointer<BitOp>> ops = BitOp::parseOps(parameters.value("take_skip_string").toString());
    if (ops.isEmpty()) {
        return nullResult;
    }

    bool frameBased =
        (parameters.contains("frame_based") && parameters.value("frame_based").toBool());
    qint64 frameCount = inputContainers.at(0)->frameCount();
    QList<QPair<QSharedPointer<const BitContainer>, qint64>> inputs;
    for (auto inputContainer : inputContainers) {
        inputs.append({inputContainer, 0});
        if (frameBased) {
            frameCount = qMin(frameCount, inputContainer->frameCount());
        }
        else {
            frameCount = 1;
        }
    }

    qint64 inputBitCount = 0;
    qint64 inputStepTotal = 0;
    qint64 outputStepTotal = 0;
    for (auto input : inputs) {
        for (int i = 0; i < frameCount; i++) {
            Frame frame;
            if (frameBased) {
                frame = input.first->frameAt(i);
            }
            else {
                frame = {Frame(input.first->bits(), 0,
                         input.first->bits()->sizeInBits() - 1)};
            }
            // need to check for integer overflow
            inputBitCount += frame.size();

            for (QSharedPointer<BitOp> op : ops) {
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
    if (parameters.contains("deinterleave_channels") && parameters.value("deinterleave_channels").toInt() > 1) {
        outputCount = parameters.value("deinterleave_channels").toInt();
    }
    QList<OutputHandle> outputs;
    QHash<QSharedPointer<BitArray>, QSharedPointer<RangeSequence>> outputFrames;
    for (int i = 0; i < outputCount; i++) {
        OutputHandle handle;
        handle.idx = 0;
        handle.bits = QSharedPointer<BitArray>(new BitArray(outputBufferSize));
        handle.frames = RangeSequence::createEmpty();
        outputs.append(handle);
    }


    qint64 totalBitsProcessed = 0;
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

                    Frame inputBits;
                    if (frameBased) {
                        inputBits = inputs.at(i).first->frameAt(currFrame);
                    }
                    else {
                        inputBits = {Frame(inputs.at(i).first->bits(), 0,
                                 inputs.at(i).first->bits()->sizeInBits() - 1)};
                    }
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
                            outputs[k].frames->appendRange(outputIdx - outStart);
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
            totalBitsProcessed += bitsProcessed;

            if (bitsProcessed > 0) {
                progress->setProgress(totalBitsProcessed, inputBitCount);
            }
            if (progress->isCancelled()) {
                return OperatorResult::error("Processing cancelled");
            }

            bitsProcessed = 0;
        }
    }

    QList<QSharedPointer<BitContainer>> outputContainers;
    int outputNum = 0;
    for (auto output: outputs) {
        output.bits->resize(output.idx);
        auto outputContainer = BitContainer::create(output.bits);
        outputContainers.append(outputContainer);
        if (frameBased) {
            outputContainer->info()->setFrames(output.frames);
        }
        if (inputContainers.size() > 1) {
            outputContainer->setName(QString("%1 Interleave").arg(parameters.value("take_skip_string").toString()));
        }
        else if (outputs.size() > 1) {
            outputContainer->setName(QString("%1 Deinterleave[%2] <- %3")
                                     .arg(parameters.value("take_skip_string").toString())
                                     .arg(outputNum)
                                     .arg(inputContainers.at(0)->name()));
        }
        else {
            outputContainer->setName(QString("%1 <- %2")
                                    .arg(parameters.value("take_skip_string").toString())
                                    .arg(inputContainers.at(0)->name()));
        }
        outputNum++;
    }
    return OperatorResult::result(outputContainers, parameters);
}


