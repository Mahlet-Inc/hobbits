#include "symbolremapper.h"
#include <QJsonArray>
#include <QObject>
#include "symbolremapperform.h"

SymbolRemapper::SymbolRemapper()
{
    QList<ParameterDelegate::ParameterInfo> infos = {
        { "mappings", ParameterDelegate::ParameterType::Array, false, {
            {"old", ParameterDelegate::ParameterType::String},
            {"new", ParameterDelegate::ParameterType::String}
          }
        }
    };

    m_delegate = ParameterDelegateUi::create(
                infos,
                [](const QJsonObject &parameters) {
                    Q_UNUSED(parameters)
                    return QString("Symbol Remap");
                },
                [](QSharedPointer<ParameterDelegate> delegate, QSize size) {
                    Q_UNUSED(size)
                    return new SymbolRemapperForm(delegate);
                });
}

OperatorInterface* SymbolRemapper::createDefaultOperator()
{
    return new SymbolRemapper();
}

QString SymbolRemapper::name()
{
    return "Symbol Remapper";
}

QString SymbolRemapper::description()
{
    return "Remaps n-bit symbols to other n-bit symbols";
}

QStringList SymbolRemapper::tags()
{
    return {"Generic"};
}

QSharedPointer<ParameterDelegate> SymbolRemapper::parameterDelegate()
{
    return m_delegate;
}

int SymbolRemapper::getMinInputContainers(const QJsonObject &pluginState)
{
    Q_UNUSED(pluginState)
    return 1;
}

int SymbolRemapper::getMaxInputContainers(const QJsonObject &pluginState)
{
    Q_UNUSED(pluginState)
    return 1;
}

QSharedPointer<const OperatorResult> SymbolRemapper::operateOnBits(
        QList<QSharedPointer<const BitContainer>> inputContainers,
        const QJsonObject &parameters,
        QSharedPointer<PluginActionProgress> progressTracker)
{

    QSharedPointer<const OperatorResult> nullResult(new OperatorResult());

    if (inputContainers.size() != 1) {
        return OperatorResult::error("Invalid input containers - expected 1.");
    }

    QStringList invalidations = m_delegate->validate(parameters);
    if (!invalidations.isEmpty()) {
        return OperatorResult::error(QString("Invalid parameters passed to %1:\n%2").arg(name()).arg(invalidations.join("\n")));
    }

    QHash<quint64, QSharedPointer<BitArray>> bitMapping;
    QJsonArray mappingsArray = parameters.value("mappings").toArray();
    int bitChunkLength = 0;
    for (auto valueRef : mappingsArray) {

        QString oldVal = valueRef.toObject().value("old").toString();
        QString newVal = valueRef.toObject().value("new").toString();

        bitChunkLength = oldVal.size();

        if (bitChunkLength > 64) {
            return OperatorResult::error("Cannot remap symbols longer than 64 bits");
        }

        QStringList parseErrors;

        auto oldBits = BitArray::fromString("0b" + oldVal, parseErrors);
        QSharedPointer<BitArray> newBits = BitArray::fromString("0b" + newVal, parseErrors);

        if (!parseErrors.isEmpty()) {
            return nullResult;
        }

        bitMapping.insert(oldBits->parseUIntValue(0, bitChunkLength), newBits);
    }

    QSharedPointer<const BitArray> inputBits = inputContainers.at(0)->bits();
    qint64 inputBitsLength = inputBits->sizeInBits();

    QSharedPointer<BitArray> outputArray = QSharedPointer<BitArray>(new BitArray(inputBitsLength));

    QJsonObject pluginState = parameters;

    for (qint64 i = 0; i < outputArray->sizeInBits() && i + bitChunkLength <= inputBits->sizeInBits(); i += bitChunkLength) {
        quint64 key = inputBits->parseUIntValue(i, bitChunkLength);
        if (bitMapping.contains(key)) {
            QSharedPointer<BitArray> mappedValue = bitMapping.value(key);
            mappedValue->copyBits(0, outputArray.data(), i, mappedValue->sizeInBits());
        }

        if (i % bitChunkLength*10 == 0) {
            progressTracker->setProgress(i, inputBitsLength);
            if (progressTracker->isCancelled()) {
                return OperatorResult::error("Processing cancelled");
            }
        }
    }

    QSharedPointer<BitContainer> container = BitContainer::create(outputArray, inputContainers.at(0)->info());

    return OperatorResult::result({container}, pluginState);
}
