#include "qamremapper.h"
#include "ui_qamremapper.h"
#include <QJsonArray>
#include <QObject>
#include <QtMath>


QamRemapper::QamRemapper() :
    ui(new Ui::QamRemapper()),
    m_remapModel(new RemapModel())
{

}

QamRemapper::~QamRemapper()
{
    delete ui;
    delete m_remapModel;
}

// Return name of operator
QString QamRemapper::getName()
{
    return "QAM Remapper";
}

void QamRemapper::provideCallback(QSharedPointer<PluginCallback> pluginCallback)
{
    // the plugin callback allows the self-triggering of operateOnContainers
    m_pluginCallback = pluginCallback;
}

void QamRemapper::applyToWidget(QWidget *widget)
{
    ui->setupUi(widget);
    m_remapModel->setRemapLength(ui->sb_bitLength->value());
    ui->tv_mappings->setModel(m_remapModel);
    connect(ui->sb_bitLength, SIGNAL(valueChanged(int)), m_remapModel, SLOT(setRemapLength(int)));
}

void QamRemapper::updateMappings()
{

}

QJsonObject QamRemapper::getStateFromUi()
{

    QJsonObject pluginState;

    // Pull data from the input fields and input them into pluginState

    QJsonArray mappingsArray;
    for (QPair<QString, QString> pair : m_remapModel->getMappings()) {
        QJsonObject pairValue;
        pairValue.insert("old", pair.first);
        pairValue.insert("new", pair.second);
        mappingsArray.append(pairValue);
    }

    pluginState.insert("mappings", mappingsArray);

    return pluginState;
}

bool QamRemapper::setPluginStateInUi(const QJsonObject &pluginState)
{
    if (!canRecallPluginState(pluginState)) {
        return false;
    }

    QJsonArray mappingsArray = pluginState.value("mappings").toArray();
    ui->sb_bitLength->setValue(int(log2(mappingsArray.size())));
    int row = 0;
    for (auto valueRef : mappingsArray) {
        QJsonObject mappingValue = valueRef.toObject();
        m_remapModel->setData(m_remapModel->index(row, 0), mappingValue.value("old").toString());
        m_remapModel->setData(m_remapModel->index(row, 1), mappingValue.value("new").toString());
        row++;
    }

    return true;
}

bool QamRemapper::canRecallPluginState(const QJsonObject &pluginState)
{

    // if pluginState does not have required fields, return false
    if (pluginState.isEmpty() == true) {
        return false;
    }

    if (!pluginState.contains("mappings") || !pluginState.value("mappings").isArray()) {
        return false;
    }

    QJsonArray mappingsArray = pluginState.value("mappings").toArray();
    for (auto valueRef : mappingsArray) {
        if (!valueRef.isObject()) {
            return false;
        }
        QJsonObject mappingValue = valueRef.toObject();
        if (!(mappingValue.contains("old")
              && mappingValue.contains("new")
              && mappingValue.value("old").isString()
              && mappingValue.value("new").isString())) {
            return false;
        }
    }

    return true;
}

int QamRemapper::getMinInputContainers(const QJsonObject &pluginState)
{
    Q_UNUSED(pluginState)
    return 1;
}

int QamRemapper::getMaxInputContainers(const QJsonObject &pluginState)
{
    Q_UNUSED(pluginState)
    return 1;
}

QSharedPointer<const OperatorResult> QamRemapper::operateOnContainers(
        QList<QSharedPointer<const BitContainer>> inputContainers,
        const QJsonObject &recallablePluginState,
        QSharedPointer<ActionProgress> progressTracker)
{

    QSharedPointer<const OperatorResult> nullResult(new OperatorResult());

    if (inputContainers.size() != 1) {
        return nullResult;
    }

    if (!canRecallPluginState(recallablePluginState)) {
        return nullResult;
    }

    QHash<BitArray, QSharedPointer<BitArray>> bitMapping;
    QJsonArray mappingsArray = recallablePluginState.value("mappings").toArray();
    int bitChunkLength = 0;
    for (auto valueRef : mappingsArray) {

        QString oldVal = valueRef.toObject().value("old").toString();
        QString newVal = valueRef.toObject().value("new").toString();

        bitChunkLength = oldVal.size();

        QStringList parseErrors;

        BitArray oldBits = *(BitArray::fromString("0b" + oldVal, parseErrors).data());
        QSharedPointer<BitArray> newBits = BitArray::fromString("0b" + newVal, parseErrors);

        if (!parseErrors.isEmpty()) {
            return nullResult;
        }

        bitMapping.insert(oldBits, newBits);
    }

    QSharedPointer<const BitArray> inputBits = inputContainers.at(0)->bits();
    qint64 inputBitsLength = inputBits->sizeInBits();

    QSharedPointer<BitArray> outputArray = QSharedPointer<BitArray>(new BitArray(inputBitsLength));

    QJsonObject pluginState = recallablePluginState;

    int lastPercent = 0;
    int failedRemappings = 0;
    BitArray inputBitChunk(bitChunkLength);
    for (int i = 0; i < outputArray->sizeInBits() && i + bitChunkLength <= inputBits->sizeInBits();
         i += bitChunkLength) {
        for (int ii = 0; ii < bitChunkLength; ii++) {
            inputBitChunk.set(ii, inputBits->at(i + ii));
        }
        if (bitMapping.contains(inputBitChunk)) {
            QSharedPointer<BitArray> mappedValue = bitMapping.value(inputBitChunk);
            for (int ii = 0; ii < mappedValue->sizeInBits(); ii++) {
                outputArray->set(i + ii, mappedValue->at(ii));
            }
        }
        else {
            failedRemappings++;
        }

        int nextPercent = int(double(i) / double(inputBitsLength) * 100.0);
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
    if (failedRemappings > 0) {
        pluginState.insert(
                "error",
                QString(
                        "Failed at the remapping of %1 sequences - the output data will be incorrect where the remapping failed").arg(
                        failedRemappings));
    }

    QSharedPointer<BitContainer> container = BitContainer::create(outputArray);
    container->setName(QString("%1 <- %2").arg("QAM Remap").arg(inputContainers.at(0)->name()));

    return OperatorResult::result({container}, pluginState);
}

void QamRemapper::previewBits(QSharedPointer<BitContainerPreview> container)
{
    Q_UNUSED(container)
}

OperatorInterface* QamRemapper::createDefaultOperator()
{
    return new QamRemapper();
}
