#include "edit.h"
#include "editeditor.h"
#include "bitarray.h"

Edit::Edit()
{
    QList<ParameterDelegate::ParameterInfo> infos = {
        // TODO: add parameters like {"myparametername", ParameterDelegate::ParameterType::Decimal}
        {"start", ParameterDelegate::ParameterType::Decimal},
        {"length", ParameterDelegate::ParameterType::Decimal},
        {"new_bits_in_range", ParameterDelegate::ParameterType::String, false},
        {"edit_type", ParameterDelegate::ParameterType::String}
    };

    m_delegate = ParameterDelegate::create(
                    infos,
                    [this](const Parameters &parameters) {
                        // TODO: use parameters to describe action better
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
    // TODO: create actual description
    return "Load bits, and modify them individually.";
}

QStringList Edit::tags()
{
    // TODO: add relevant tags
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

        /*

        QByteArray nair = newBitsInRange.toLocal8Bit();
        const char * newAsciiInRange = nair.data();

        int newBitsInRangeLength = newBitsInRange.length();
        qint64 outputSize = inputContainerSize - (length * 8) + (newBitsInRangeLength * 8);

        QSharedPointer<const BitArray> bits = inputContainers.at(0)->bits();

        if (newBitsInRangeLength == length) {     
            m_outBits->setBytes(start, newAsciiInRange, 0, newBitsInRangeLength);
        } else {
            QSharedPointer<BitArray> postBits = QSharedPointer<BitArray>(new BitArray(outputSize - start + length ));
            QByteArray byteArray = bits->readBytes(start + length, outputSize - 1);
            int endLength = byteArray.length();
            const char* data = byteArray.data();
            m_outBits->setBytes(start+newBitsInRangeLength, data, 0, endLength);
            m_outBits->setBytes(start, newAsciiInRange, 0, newBitsInRangeLength);
            
        }

        */

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

    //pte_bits is already filled with data from start to length
    //
    //auto bitContainer = inputContainers.first();
    QList<QSharedPointer<BitContainer>> outputContainers;

    

    

    //do something with bits here
    //ex 0000 1111 -> 1111 0000

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

    //for each bit in the bit container
    //if bit index is less than the start index, or greater than the end index, copy the bit exactly

    auto bitContainer = BitContainer::create(outBits);

    outputContainers.append(bitContainer);
    //return OperatorResult::error("Hej");
    return OperatorResult::result(outputContainers, parameters);
    // TODO: Perform operation and return result with OperatorResult::result
    //return OperatorResult::error("Bits from start to length would be changed according to your entry, but that is not implemented yet.");
}

