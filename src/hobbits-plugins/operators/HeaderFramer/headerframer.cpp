#include "bitarray.h"
#include "headerframer.h"
#include <QJsonArray>
#include <QObject>
#include "headerframerform.h"

HeaderFramer::HeaderFramer()
{
    QList<ParameterDelegate::ParameterInfo> infos = {
        { "headers", QJsonValue::Array, false, {
            {"header", QJsonValue::String},
            {"length", QJsonValue::String},
            {"pre-pad", QJsonValue::Double, true},
            {"byte-aligned", QJsonValue::Bool, true}
          }
        }
    };

    m_delegate = ParameterDelegateUi::create(
                    infos,
                    [](const QJsonObject &parameters) {
                        Q_UNUSED(parameters)
                        return QString("Frame by Header");
                    },
                    [](QSharedPointer<ParameterDelegate> delegate, QSize size) {
                        Q_UNUSED(size)
                        return new HeaderFramerForm(delegate);
                    });
}

OperatorInterface* HeaderFramer::createDefaultOperator()
{
    return new HeaderFramer();
}

QString HeaderFramer::name()
{
    return "Header Framer";
}

QString HeaderFramer::description()
{
    return "Frame data based on constant headers or alignment values";
}

QStringList HeaderFramer::tags()
{
    return {"Generic"};
}

QSharedPointer<ParameterDelegate> HeaderFramer::parameterDelegate()
{
    return m_delegate;
}

int HeaderFramer::getMinInputContainers(const QJsonObject &pluginState)
{
    Q_UNUSED(pluginState)
    return 1;
}

int HeaderFramer::getMaxInputContainers(const QJsonObject &pluginState)
{
    Q_UNUSED(pluginState)
    return 1;
}

bool headerGreaterThan(HeaderFramer::HeaderInfo a, HeaderFramer::HeaderInfo b)
{
    return a.headerBits->sizeInBits() > b.headerBits->sizeInBits();
}

QSharedPointer<const OperatorResult> HeaderFramer::operateOnBits(
        QList<QSharedPointer<const BitContainer>> inputContainers,
        const QJsonObject &parameters,
        QSharedPointer<PluginActionProgress> progressTracker)
{

    QSharedPointer<OperatorResult> result(new OperatorResult());

    if (!m_delegate->validate(parameters)) {
        return OperatorResult::error("Invalid parameters given to Header Framer");
    }

    if (inputContainers.size() != 1) {
        return OperatorResult::error(QString("Header Framer requires only 1 input bit container.  Was given %1").arg(inputContainers.size()));
    }

    QList<HeaderInfo> headers;
    QJsonArray headersArray = parameters.value("headers").toArray();
    for (auto valueRef : headersArray) {
        QJsonObject headerData = valueRef.toObject();
        HeaderInfo headerInfo = HeaderInfo();
        QStringList parseErrors;
        headerInfo.headerBits = BitArray::fromString(headerData.value("header").toString());
        if (!parseErrors.isEmpty()) {
            result->setParameters(QJsonObject({QPair<QString, QJsonValue>("error", parseErrors.join("\n"))}));
            return OperatorResult::error(QString("Invalid Bit Array string in header parameters: '%1'").arg(headerData.value("header").toString()));
        }
        QString lengthString = headerData.value("length").toString();
        if (lengthString == "*") {
            headerInfo.frameLength = -1;
        }
        else {
            bool ok;
            headerInfo.frameLength = lengthString.toInt(&ok);
            if (!ok) {
                return OperatorResult::error(QString("Invalid header length value '%1'").arg(lengthString));
            }
        }
        if (headerData.contains("pre-pad") && headerData.value("pre-pad").isDouble()) {
            headerInfo.prePadLength = headerData.value("pre-pad").toInt();
        }
        else {
            headerInfo.prePadLength = 0;
        }
        if (headerData.contains("byte-aligned") && headerData.value("byte-aligned").isBool()) {
            headerInfo.byteAligned = headerData.value("byte-aligned").toBool();
        }
        else {
            headerInfo.byteAligned = false;
        }
        headers.append(headerInfo);
    }

    std::sort(headers.begin(), headers.end(), headerGreaterThan);

    QSharedPointer<const BitArray> bits = inputContainers.at(0)->bits();

    auto frames = RangeSequence::createEmpty();
    auto outputBits = QSharedPointer<BitArray>(new BitArray(bits->sizeInBits()));
    qint64 start = 0;
    qint64 pos = 0;
    qint64 outputSize = 0;
    bool buildingFrame = false;
    for ( ; pos < bits->sizeInBits(); pos++) {
        for (auto headerInfo : headers) {
            if (headerInfo.byteAligned) {
                if (pos % 8 != 0) {
                    continue;
                }
            }
            QSharedPointer<BitArray> header = headerInfo.headerBits;
            if (header->sizeInBits() + headerInfo.prePadLength > bits->sizeInBits() - pos) {
                break;
            }
            bool match = true;
            for (qint64 i = 0; i < header->sizeInBits() && pos + i + headerInfo.prePadLength < bits->sizeInBits(); i++) {
                if (bits->at(pos + i + headerInfo.prePadLength) != header->at(i)) {
                    match = false;
                    break;
                }
            }

            if (match) {
                // We found a header that starts at 'end'
                if (buildingFrame) {
                    // A previous header had an open-ended length and this match finishes that frame
                    qint64 frameSize = pos - start;
                    bits->copyBits(start, outputBits.data(), frames->getValueCount(), frameSize, BitArray::Or);
                    frames->appendRange(frameSize);
                    outputSize += frameSize;
                }
                if (headerInfo.frameLength > 0) {
                    // This header's frame has a static length, so we can just add the whole frame
                    buildingFrame = false;
                    qint64 end = pos + headerInfo.frameLength - 1;
                    if (end < bits->sizeInBits()) {
                        qint64 frameSize = headerInfo.frameLength;
                        bits->copyBits(pos, outputBits.data(), frames->getValueCount(), frameSize, BitArray::Or);
                        frames->appendRange(frameSize);
                        outputSize += frameSize;
                    }
                    pos = end;
                }
                else {
                    // This header's frame has an open-ended length, so we need to find the start of the next frame before adding it
                    start = pos;
                    pos = pos + header->sizeInBits() - 1 + headerInfo.prePadLength;
                    buildingFrame = true;
                }
                break;
            }
        }

        progressTracker->setProgress(pos, bits->sizeInBits());
        if (progressTracker->isCancelled()) {
            return OperatorResult::error("Processing Cancelled");
        }
    }
    if (buildingFrame) {
        qint64 frameSize = bits->sizeInBits() - start;
        bits->copyBits(start, outputBits.data(), frames->getValueCount(), frameSize, BitArray::Or);
        frames->appendRange(frameSize);
        outputSize += frameSize;
    }
    outputBits->resize(outputSize);

    QSharedPointer<BitContainer> outputContainer = BitContainer::create(outputBits);
    outputContainer->info()->setFrames(frames);

    return OperatorResult::result({outputContainer}, parameters);
}
