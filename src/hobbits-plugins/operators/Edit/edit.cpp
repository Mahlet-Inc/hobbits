#include "edit.h"
#include "editeditor.h"
#include "bitarray.h"

Edit::Edit()
{
    QList<ParameterDelegate::ParameterInfo> infos = {
        {"start", ParameterDelegate::ParameterType::Decimal},
        {"length", ParameterDelegate::ParameterType::Decimal},
        {"new_bits_in_range", ParameterDelegate::ParameterType::String, false},
        {"edit_type", ParameterDelegate::ParameterType::String}
    };

    m_delegate = ParameterDelegate::create(
                    infos,
                    [this](const Parameters &parameters) {
                        return QString("Apply %1").arg(this->name());
                    },
                    [](QSharedPointer<ParameterDelegate> delegate, QSize size) {
                        Q_UNUSED(size)
                        return new EditEditor(delegate);
                    });
}

OperatorInterface* Edit::createDefaultOperator()
{
    return new Edit();
}

QString Edit::name()
{
    return "Edit";
}

QString Edit::description()
{
    return "Directly replace or insert into segments of a bit container";
}

QStringList Edit::tags()
{
    return {"Generic"};
}

QSharedPointer<ParameterDelegate>  Edit::parameterDelegate()
{
    return m_delegate;
}

int Edit::getMinInputContainers(const Parameters &parameters)
{
    Q_UNUSED(parameters)
    return 1;
}

int Edit::getMaxInputContainers(const Parameters &parameters)
{
    Q_UNUSED(parameters)
    return 1;
}

int Edit::getUnitSize(Parameters parameters) {
    if (parameters.value("edit_type").toString() == "hex") {
        return 4;
    }
    else if (parameters.value("edit_type").toString() == "ascii") {
        return 8;
    }
    else {
        return 1;
    }
}

QSharedPointer<BitArray> Edit::parseBits(QString newBitsInRange) 
{
    QStringList parseErrors = QStringList();
    QSharedPointer<BitArray> newBitArray;
    QString bitArraySpec = "0b"+newBitsInRange;
    QSharedPointer<BitArray> newBits = newBitArray->fromString(bitArraySpec, &parseErrors);
    return newBits;
}

QSharedPointer<BitArray> Edit::parseHex(QString newBitsInRange) 
{
    QStringList parseErrors = QStringList();
    QSharedPointer<BitArray> newBitArray;
    QString bitArraySpec = "0x"+newBitsInRange;
    QSharedPointer<BitArray> newBits = newBitArray->fromString(bitArraySpec, &parseErrors);
    return newBits;
}

QSharedPointer<BitArray> Edit::parseAscii(QString newBitsInRange) {
        QStringList parseErrors = QStringList();
        QSharedPointer<BitArray> newBitArray;
        QString bitArraySpec = newBitsInRange;
        QSharedPointer<BitArray> newBits = newBitArray->fromString(bitArraySpec, &parseErrors);
        return newBits;
}

QSharedPointer<const OperatorResult> Edit::operateOnBits(
    QList<QSharedPointer<const BitContainer> > inputContainers,
    const Parameters &parameters,
    QSharedPointer<PluginActionProgress> progress)
{    
    if (inputContainers.length() == 0) {
        QList<QSharedPointer<BitContainer>> outputContainers;
        qint64 outputSize = 10;
        QSharedPointer<BitArray> outBits;
        outBits = QSharedPointer<BitArray>(new BitArray(outputSize));

        auto bitContainer = BitContainer::create(outBits);

        outputContainers.append(bitContainer);
        return OperatorResult::result(outputContainers, parameters);
    }

    if (inputContainers.length() > 1) {
        return OperatorResult::error("Requires a single input bit container");
        
    }

    QStringList invalidations = m_delegate->validate(parameters);
    if (!invalidations.isEmpty()) {
        return OperatorResult::error(QString("Invalid parameters passed to %1:\n%2").arg(name()).arg(invalidations.join("\n")));
    }

    int progressInt = 15;

    progress->setProgressPercent(progressInt);

    //get values from spin boxes
    qint64 start = parameters.value("start").toInt();
    qint64 length = parameters.value("length").toInt();

    //get bits from text box
    QString newBitsInRange = parameters.value("new_bits_in_range").toString();
    QList<QSharedPointer<BitContainer>> outputContainers;

    int end = start + length;
    qint64 inputContainerSize = inputContainers.at(0)->size();
    QSharedPointer<const BitArray> bits = inputContainers.at(0)->bits();

    progressInt+=15; progress->setProgressPercent(progressInt);

    int newBitsInRangeLength = newBitsInRange.length();

    int unitSize = getUnitSize(parameters);

    qint64 outputSize = inputContainerSize - (length * unitSize) + (newBitsInRangeLength * unitSize);
    
    QSharedPointer<BitArray> outBits = QSharedPointer<BitArray>(new BitArray(outputSize));

    bits->copyBits(0, outBits.data(), 0, start * unitSize, BitArray::Copy);

    bits->copyBits((qint64) end * unitSize, outBits.data(), (qint64)((qint64)end + newBitsInRangeLength - length)*unitSize, 
        outputSize, BitArray::Copy);
    progressInt+=45; progress->setProgressPercent(progressInt);

    QSharedPointer<BitArray> newBits;

    if (parameters.value("edit_type").toString() == "bit" ) { //bits
        QRegExp re("^[0-1]*$");
        if (!re.exactMatch(newBitsInRange))
            return OperatorResult::error("You can only use 0 or 1 in bit mode.");
       newBits  = parseBits( newBitsInRange);

        
    } else if (parameters.value("edit_type").toString() == "hex") { //hex
        QRegExp re("^[a-f 0-9]*$");
        if (!re.exactMatch(newBitsInRange))
            return OperatorResult::error("Invalid hex character.");
        newBits = parseHex( newBitsInRange);
        

    } else if (parameters.value("edit_type").toString() == "ascii") { //ascii

        newBits = parseAscii( newBitsInRange);
        
    }

    newBits->copyBits(0, outBits.data(), start*unitSize, newBits->sizeInBits()*unitSize, BitArray::Copy);
    progressInt+=15; progress->setProgressPercent(progressInt);

    auto bitContainer = BitContainer::create(outBits);

    outputContainers.append(bitContainer);
    return OperatorResult::result(outputContainers, parameters);
}

