#include "bitarray.h"
#include "headerframer.h"
#include "ui_headerframer.h"
#include <QJsonArray>
#include <QObject>


HeaderFramer::HeaderFramer() :
    ui(new Ui::HeaderFramer())
{

}

// Return name of operator
QString HeaderFramer::getName()
{
    return "Header Framer";
}

void HeaderFramer::provideCallback(QSharedPointer<PluginCallback> pluginCallback)
{
    // the plugin callback allows the self-triggering of operateOnContainers
    m_pluginCallback = pluginCallback;
}

void HeaderFramer::applyToWidget(QWidget *widget)
{
    ui->setupUi(widget);

    connect(ui->le_header, SIGNAL(textChanged(QString)), this, SLOT(validateHeader(QString)));
    connect(ui->cb_fixedLength, SIGNAL(stateChanged(int)), this, SLOT(fixedLengthToggled(int)));

    connect(ui->pb_add, SIGNAL(pressed()), this, SLOT(addHeader()));
    connect(ui->pb_remove, SIGNAL(pressed()), this, SLOT(removeHeader()));

    connect(ui->tw_headers, SIGNAL(itemSelectionChanged()), this, SLOT(checkSelectedHeader()));

    ui->pb_add->setEnabled(false);
    ui->pb_remove->setEnabled(false);
    ui->sb_frameLength->setEnabled(false);
    ui->tw_headers->setColumnCount(2);
    ui->tw_headers->setHorizontalHeaderLabels({"Frame Header", "Frame Length"});
    ui->sb_frameLength->setMaximum(INT32_MAX);
}

QString HeaderFramer::getHeaderString()
{
    // Parse header
    if (ui->le_header->text().isEmpty()) {
        return QString();
    }

    QStringList parseErrors;
    QSharedPointer<BitArray> header = BitArray::fromString(ui->le_header->text(), parseErrors);
    if (!parseErrors.isEmpty()) {
        return QString();
    }

    ui->sb_frameLength->setMinimum(int(header->sizeInBits()));

    return ui->le_header->text();
}

void HeaderFramer::fixedLengthToggled(int fixed)
{
    ui->sb_frameLength->setEnabled(fixed == Qt::Checked);
}

void HeaderFramer::validateHeader(QString header)
{
    Q_UNUSED(header)
    QString headerString = getHeaderString();
    ui->pb_add->setEnabled(!headerString.isEmpty());
}

void HeaderFramer::addHeader()
{
    QString headerString = getHeaderString();
    if (headerString.isEmpty()) {
        return;
    }

    int row = ui->tw_headers->rowCount();
    ui->tw_headers->insertRow(row);
    ui->tw_headers->setItem(row, 0, new QTableWidgetItem(headerString));
    if (ui->cb_fixedLength->isChecked()) {
        ui->tw_headers->setItem(row, 1, new QTableWidgetItem(QString("%1").arg(ui->sb_frameLength->value())));
    }
    else {
        ui->tw_headers->setItem(row, 1, new QTableWidgetItem("*"));
    }
}

void HeaderFramer::checkSelectedHeader()
{
    ui->pb_remove->setEnabled(!ui->tw_headers->selectedItems().isEmpty());
}

void HeaderFramer::removeHeader()
{
    QSet<int> rows;
    for (auto item : ui->tw_headers->selectedItems()) {
        rows.insert(item->row());
    }
    QList<int> rowList = rows.values();
    std::sort(rowList.begin(), rowList.end());
    for (int i = rowList.length() - 1; i >= 0; i--) {
        ui->tw_headers->removeRow(rowList.at(i));
    }
}

bool HeaderFramer::canRecallPluginState(const QJsonObject &pluginState)
{
    // if pluginState does not have required fields, return false
    if (pluginState.isEmpty() == true) {
        return false;
    }

    if (!pluginState.contains("headers") || !pluginState.value("headers").isArray()) {
        return false;
    }

    QJsonArray mappingsArray = pluginState.value("headers").toArray();
    if (mappingsArray.isEmpty()) {
        return false;
    }
    for (auto valueRef : mappingsArray) {
        if (!valueRef.isObject()) {
            return false;
        }
        QJsonObject mappingValue = valueRef.toObject();
        if (!(mappingValue.contains("header")
              && mappingValue.contains("length")
              && mappingValue.value("header").isString()
              && mappingValue.value("length").isString())) {
            return false;
        }
    }

    return true;
}

QJsonObject HeaderFramer::getStateFromUi()
{
    QJsonObject pluginState;

    QJsonArray headersArray;
    for (int i = 0; i < ui->tw_headers->rowCount(); i++) {
        QJsonObject headerData;
        headerData.insert("header", ui->tw_headers->item(i, 0)->text());
        headerData.insert("length", ui->tw_headers->item(i, 1)->text());
        headersArray.append(headerData);
    }

    pluginState.insert("headers", headersArray);

    return pluginState;
}

bool HeaderFramer::setPluginStateInUi(const QJsonObject &pluginState)
{
    if (!canRecallPluginState(pluginState)) {
        return false;
    }

    QJsonArray headersArray = pluginState.value("headers").toArray();
    int row = 0;
    ui->tw_headers->clearContents();
    ui->tw_headers->setRowCount(0);
    for (auto valueRef : headersArray) {
        ui->tw_headers->insertRow(ui->tw_headers->rowCount());
        QJsonObject headerData = valueRef.toObject();
        ui->tw_headers->setItem(row, 0, new QTableWidgetItem(headerData.value("header").toString()));
        ui->tw_headers->setItem(row, 1, new QTableWidgetItem(headerData.value("length").toString()));
        row++;
    }

    return true;
}

int HeaderFramer::getMinInputContainers(const QJsonObject &pluginState)
{
    return 1;
}

int HeaderFramer::getMaxInputContainers(const QJsonObject &pluginState)
{
    return 1;
}

bool headerGreaterThan(QPair<QSharedPointer<BitArray>, int> a, QPair<QSharedPointer<BitArray>, int> b)
{
    return a.first->sizeInBits() > b.first->sizeInBits();
}

QSharedPointer<const OperatorResult> HeaderFramer::operateOnContainers(
        QList<QSharedPointer<const BitContainer>> inputContainers,
        const QJsonObject &recallablePluginState,
        QSharedPointer<ActionProgress> progressTracker)
{

    QSharedPointer<OperatorResult> result(new OperatorResult());

    if (!canRecallPluginState(recallablePluginState)) {
        return result;
    }

    if (inputContainers.size() != 1) {
        return result;
    }

    QList<QPair<QSharedPointer<BitArray>, int>> headers;
    QJsonArray headersArray = recallablePluginState.value("headers").toArray();
    for (auto valueRef : headersArray) {
        QJsonObject headerData = valueRef.toObject();
        QStringList parseErrors;
        QSharedPointer<BitArray> header = BitArray::fromString(headerData.value("header").toString());
        if (!parseErrors.isEmpty()) {
            result->setPluginState(QJsonObject({QPair<QString, QJsonValue>("error", parseErrors.join("\n"))}));
            return result;
        }
        QString lengthString = headerData.value("length").toString();
        int length;
        if (lengthString == "*") {
            length = -1;
        }
        else {
            bool ok;
            length = lengthString.toInt(&ok);
            if (!ok) {
                result->setPluginState(
                        QJsonObject(
                                {QPair<QString, QJsonValue>(
                                        "error",
                                        QString("Invalid header length value '%1'").arg(lengthString))}));
                return result;
            }
        }
        headers.append(QPair<QSharedPointer<BitArray>, int>(header, length));
    }

    std::sort(headers.begin(), headers.end(), headerGreaterThan);

    QSharedPointer<const BitArray> bits = inputContainers.at(0)->getBaseBits();

    QList<Frame> frames;
    qint64 maxFrameWidth = 0;
    int lastPercent = 0;
    qint64 start = 0;
    qint64 pos = 0;
    qint64 outputSize = 0;
    bool buildingFrame = false;
    for ( ; pos < bits->sizeInBits(); pos++) {
        for (auto headerInfo : headers) {
            QSharedPointer<BitArray> header = headerInfo.first;
            if (header->sizeInBits() > bits->sizeInBits() - pos) {
                break;
            }
            bool match = true;
            for (qint64 i = 0; i < header->sizeInBits() && pos + i < bits->sizeInBits(); i++) {
                if (bits->at(pos + i) != header->at(i)) {
                    match = false;
                    break;
                }
            }

            if (match) {
                // We found a header that starts at 'end'
                if (buildingFrame) {
                    // A previous header had an open-ended length and this match finishes that frame
                    frames.append(Frame(bits, start, pos - 1));
                    maxFrameWidth = qMax(maxFrameWidth, frames.last().size());
                    outputSize += frames.last().size();
                }
                if (headerInfo.second > 0) {
                    // This header's frame has a static length, so we can just add the whole frame
                    buildingFrame = false;
                    qint64 end = pos + headerInfo.second - 1;
                    if (end < bits->sizeInBits()) {
                        frames.append(Frame(bits, pos, end));
                        maxFrameWidth = qMax(maxFrameWidth, frames.last().size());
                        outputSize += frames.last().size();
                    }
                    pos = end;
                }
                else {
                    // This header's frame has an open-ended length, so we need to find the start of the next frame before adding it
                    start = pos;
                    pos = pos + header->sizeInBits() - 1;
                    buildingFrame = true;
                }
                break;
            }
        }

        int nextPercent = int(double(pos) / double(bits->sizeInBits()) * 100.0);
        if (nextPercent > lastPercent) {
            lastPercent = nextPercent;
            progressTracker->setProgressPercent(nextPercent);
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
    if (buildingFrame) {
        frames.append(Frame(bits, start, bits->sizeInBits() - 1));
        maxFrameWidth = qMax(maxFrameWidth, frames.last().size());
        outputSize += frames.last().size();
    }

    QSharedPointer<BitArray> outputBits = QSharedPointer<BitArray>(new BitArray(outputSize));
    QList<Range> outputFrames;
    int outputIndex = 0;
    for (Frame frame : frames) {
        for (qint64 i = 0; i < frame.size(); i++) {
            outputBits->set(outputIndex + i, frame.at(i));
        }
        outputFrames.append(Range(outputIndex, outputIndex + frame.size() - 1));
        outputIndex += frame.size();
    }

    QSharedPointer<BitContainer> outputContainer = QSharedPointer<BitContainer>(new BitContainer());
    outputContainer->setBytes(outputBits);
    outputContainer->setHighlights("frames", outputFrames);
    outputContainer->setMetadata("max_frame_width", {QString("%1").arg(maxFrameWidth)});
    outputContainer->frameViaHighlights();

    result->setPluginState(recallablePluginState);
    result->setOutputContainers({outputContainer});

    return result;
}

OperatorInterface* HeaderFramer::createDefaultOperator()
{
    return new HeaderFramer();
}
