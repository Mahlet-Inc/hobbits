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

    connect(ui->pb_add, SIGNAL(pressed()), this, SLOT(addHeader()));
    connect(ui->pb_remove, SIGNAL(pressed()), this, SLOT(removeHeader()));

    connect(ui->tw_headers, SIGNAL(itemSelectionChanged()), this, SLOT(checkSelectedHeader()));

    ui->pb_add->setEnabled(false);
    ui->pb_remove->setEnabled(false);
    ui->tw_headers->setColumnCount(4);
    ui->tw_headers->setHorizontalHeaderLabels({"Frame Header", "Frame Length", "Pre-pad", "Byte-aligned"});
    ui->sb_frameLength->setMaximum(INT32_MAX);

    ui->sb_prePad->setMinimum(0);
    ui->sb_prePad->setMaximum(INT32_MAX);

    connect(ui->cb_prePad, SIGNAL(stateChanged(int)), this, SLOT(showSpinBoxes()));
    connect(ui->cb_fixedLength, SIGNAL(stateChanged(int)), this, SLOT(showSpinBoxes()));
    showSpinBoxes();
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

void HeaderFramer::showSpinBoxes()
{
    if (ui->cb_prePad->isChecked()) {
        ui->sb_prePad->setVisible(true);
        ui->cb_prePad->setText("Pre-pad:");
    }
    else {
        ui->sb_prePad->setVisible(false);
        ui->cb_prePad->setText("Pre-pad");
    }
    if (ui->cb_fixedLength->isChecked()) {
        ui->sb_frameLength->setVisible(true);
        ui->cb_fixedLength->setText("Fixed Frame Length:");
    }
    else {
        ui->sb_frameLength->setVisible(false);
        ui->cb_fixedLength->setText("Fixed Frame Length");
    }
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

    if (ui->cb_prePad->isChecked()) {
        auto item = new QTableWidgetItem(ui->sb_prePad->text());
        item->setData(Qt::UserRole, ui->sb_prePad->value());
        ui->tw_headers->setItem(row, 2, item);
    }
    else {
        auto item = new QTableWidgetItem("0");
        item->setData(Qt::UserRole, 0);
        ui->tw_headers->setItem(row, 2, item);
    }

    if (ui->cb_byteAlign->isChecked()) {
        auto item = new QTableWidgetItem("true");
        item->setData(Qt::UserRole, true);
        ui->tw_headers->setItem(row, 3, item);
    }
    else {
        auto item = new QTableWidgetItem("false");
        item->setData(Qt::UserRole, false);
        ui->tw_headers->setItem(row, 3, item);
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
        headerData.insert("pre-pad", ui->tw_headers->item(i, 2)->data(Qt::UserRole).toInt());
        headerData.insert("byte-aligned", ui->tw_headers->item(i, 3)->data(Qt::UserRole).toBool());
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

        if (headerData.contains("pre-pad") && headerData.value("pre-pad").isDouble()) {
            auto item = new QTableWidgetItem(QString("%1").arg(headerData.value("pre-pad").toDouble()));
            item->setData(Qt::UserRole, headerData.value("pre-pad"));
            ui->tw_headers->setItem(row, 2, item);
        }
        if (headerData.contains("byte-aligned") && headerData.value("byte-aligned").isBool()) {
            auto item = new QTableWidgetItem(headerData.value("byte-aligned").toBool() ? "true" : "false");
            item->setData(Qt::UserRole, headerData.value("byte-aligned"));
            ui->tw_headers->setItem(row, 3, item);
        }

        row++;
    }

    return true;
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

QSharedPointer<const OperatorResult> HeaderFramer::operateOnContainers(
        QList<QSharedPointer<const BitContainer>> inputContainers,
        const QJsonObject &recallablePluginState,
        QSharedPointer<ActionProgress> progressTracker)
{

    QSharedPointer<OperatorResult> result(new OperatorResult());

    if (!canRecallPluginState(recallablePluginState)) {
        return OperatorResult::error("Invalid parameters given to Header Framer");
    }

    if (inputContainers.size() != 1) {
        return OperatorResult::error(QString("Header Framer requires only 1 input bit container.  Was given %1").arg(inputContainers.size()));
    }

    QList<HeaderInfo> headers;
    QJsonArray headersArray = recallablePluginState.value("headers").toArray();
    for (auto valueRef : headersArray) {
        QJsonObject headerData = valueRef.toObject();
        HeaderInfo headerInfo = HeaderInfo();
        QStringList parseErrors;
        headerInfo.headerBits = BitArray::fromString(headerData.value("header").toString());
        if (!parseErrors.isEmpty()) {
            result->setPluginState(QJsonObject({QPair<QString, QJsonValue>("error", parseErrors.join("\n"))}));
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

    QList<Frame> frames;
    qint64 maxFrameWidth = 0;
    int lastPercent = 0;
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
                    frames.append(Frame(bits, start, pos - 1));
                    maxFrameWidth = qMax(maxFrameWidth, frames.last().size());
                    outputSize += frames.last().size();
                }
                if (headerInfo.frameLength > 0) {
                    // This header's frame has a static length, so we can just add the whole frame
                    buildingFrame = false;
                    qint64 end = pos + headerInfo.frameLength - 1;
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
                    pos = pos + header->sizeInBits() - 1 + headerInfo.prePadLength;
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
    QSharedPointer<BitInfo> bitInfo(new BitInfo);
    bitInfo->setFrames(outputFrames);
    outputContainer->setBits(outputBits, bitInfo);
    outputContainer->setName(QString("h-framed <- %1").arg(inputContainers.at(0)->name()));

    return OperatorResult::result({outputContainer}, recallablePluginState);
}

void HeaderFramer::previewBits(QSharedPointer<BitContainerPreview> container)
{
    Q_UNUSED(container)
}

OperatorInterface* HeaderFramer::createDefaultOperator()
{
    return new HeaderFramer();
}
