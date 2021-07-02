#include "edit.h"
#include "editeditor.h"

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

QSharedPointer<const OperatorResult> Edit::operateOnBits(
    QList<QSharedPointer<const BitContainer> > inputContainers,
    const Parameters &parameters,
    QSharedPointer<PluginActionProgress> progress)
{    
    if (inputContainers.length() == 0) {
        QList<QSharedPointer<BitContainer>> outputContainers;
        int outputSize = 10;
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
    //get values from spin boxes
    qint64 start = parameters.value("start").toInt();
    qint64 length = parameters.value("length").toInt();


    //get bits from text box
    QString newBitsInRange = parameters.value("new_bits_in_range").toString();

    //TODO: pte_bits is already filled with data from start to length
    //
    //auto bitContainer = inputContainers.first();
    QList<QSharedPointer<BitContainer>> outputContainers;
    QSharedPointer<BitArray> outBits;

    

    //do something with bits here
    //ex 0000 1111 -> 1111 0000

    int end = start + length;
    int inputContainerSize = inputContainers.at(0)->size();
    QSharedPointer<const BitArray> bits = inputContainers.at(0)->bits();
    int newBitsInRangeLength = newBitsInRange.length();
    

    if (parameters.value("edit_type").toString() == "bit") { //bits

        int outputSize = inputContainerSize - length + newBitsInRangeLength;
        outBits = QSharedPointer<BitArray>(new BitArray(outputSize));
        bits->copyBits(0, outBits.data(), 0, outputSize, BitArray::Copy);

        
    
        int j = 0; //j represents the index of the newBitsInRange string
        int i2 = 0; //the index from the input container
        bool setiToEnd = false;
        bool checkSeti = true;

        for (int i = 0; i < outputSize; i ++) { //i represents the index of the outBits, soon to be outputContainer
            if (i < start || i > start + newBitsInRangeLength - 1) {
                if (newBitsInRangeLength == 0 && i >= start && checkSeti) { //if input is empty set index from input container to ending index
                    setiToEnd = true;
                    checkSeti = false;
                }
                if (setiToEnd) {
                    i2 = end;
                    setiToEnd = false;
                }
                if (i2 < inputContainerSize) {
                    outBits->set(i, inputContainers.at(0)->bits()->at(i2)); //copy bits from input container
                }
                i2++;
            } else {
                const QChar x = newBitsInRange.at(j);
                if (x.digitValue() == 0)
                    outBits->set(i, false); //false is 0, true is 1
                else if (x.digitValue() == 1)
                    outBits->set(i, true); //false is 0, true is 1
                else
                    return OperatorResult::error("You can only use 0 or 1 in bit mode.");
                j++;
                setiToEnd = true;
            }
        }
    } else if (parameters.value("edit_type").toString() == "hex") { //hex

        int outputSize = inputContainerSize - (length * 4) + (newBitsInRangeLength * 4);
        outBits = QSharedPointer<BitArray>(new BitArray(outputSize));
        bits->copyBits(0, outBits.data(), 0, outputSize, BitArray::Copy);

        int i2 = 0;
        int j = 0;
        int ji = 0; //increment 4 times before incrementing j
        bool setiToEnd = false;
        bool checkSeti = true;
        for (int i = 0; i < outputSize; i++) {
            if (i < start * 4 || i > (start * 4) + ((newBitsInRangeLength * 4)-1)) {
                
                if (newBitsInRangeLength == 0 && i >= start && checkSeti) { //if input is empty set index from input container to ending index
                    setiToEnd = true;
                    checkSeti = false;
                }
                if (setiToEnd) {
                    i2 = end * 4;
                    setiToEnd = false;
                }
                if (i2 < inputContainerSize) {
                    outBits->set(i, inputContainers.at(0)->bits()->at(i2)); //copy bits from input container
                }

            } else {
                
                const QChar x = newBitsInRange.at(j);
                
                if (x == '0') {
                    outBits->set(i, false); //false is 0, true is 1
                } else
                if (x == '1') {
                    if (ji < 3) { 
                        outBits->set(i, false); //false is 0, true is 1
                    } else {
                        outBits->set(i, true); //false is 0, true is 1
                    }
                } else
                if (x == '2') {
                    if (ji < 2 || ji > 2) { 
                        outBits->set(i, false); //false is 0, true is 1
                    } else {
                        outBits->set(i, true); //false is 0, true is 1
                    }
                } else
                if (x == '3') {
                    if (ji == 2 || ji == 3) { 
                        outBits->set(i, true); //false is 0, true is 1
                    } else {
                        outBits->set(i, false); //false is 0, true is 1
                    }
                } else


                if (x == '4') {
                    if (ji == 1) { 
                        outBits->set(i, true); //false is 0, true is 1
                    } else {
                        outBits->set(i, false); //false is 0, true is 1
                    }
                } else
                if (x == '5') {
                    if (ji == 1 || ji == 3) { 
                        outBits->set(i, true); //false is 0, true is 1
                    } else {
                        outBits->set(i, false); //false is 0, true is 1
                    }
                } else
                if (x == '6') {
                    if (ji == 1 || ji == 2) { 
                        outBits->set(i, true); //false is 0, true is 1
                    } else {
                        outBits->set(i, false); //false is 0, true is 1
                    }
                } else
                if (x == '7') {
                    if (ji > 0) { 
                        outBits->set(i, true); //false is 0, true is 1
                    } else {
                        outBits->set(i, false); //false is 0, true is 1
                    }
                } else


                if (x == '8') {
                    if (ji == 0) {
                        outBits->set(i, true);
                    } else {
                        outBits->set(i, false); //false is 0, true is 1
                    }
                } else
                if (x == '9') {
                    if (ji == 0 || ji > 2) {
                        outBits->set(i, true);
                    } else {
                        outBits->set(i, false); //false is 0, true is 1
                    }
                } else
                if (x == 'a') {
                    if (ji == 0 || ji == 2) {
                        outBits->set(i, true);
                    } else {
                        outBits->set(i, false); //false is 0, true is 1
                    }
                } else
                if (x == 'b') {
                    if (ji == 1) { 
                        outBits->set(i, false); //false is 0, true is 1
                    } else {
                        outBits->set(i, true); //false is 0, true is 1
                    }
                } else


                if (x == 'c') {
                    if (ji < 2) { 
                        outBits->set(i, true); //false is 0, true is 1
                    } else {
                        outBits->set(i, false); //false is 0, true is 1
                    }
                } else
                if (x == 'd') {
                    if (ji == 2) { 
                        outBits->set(i, false); //false is 0, true is 1
                    } else {
                        outBits->set(i, true); //false is 0, true is 1
                    }
                } else
                if (x == 'e') {
                    if (ji == 3) { 
                        outBits->set(i, false); //false is 0, true is 1
                    } else {
                        outBits->set(i, true); //false is 0, true is 1
                    }
                } else
                if (x == 'f') {
                    outBits->set(i, true); //false is 0, true is 1
                } else {
                    QString str = " is an invalid character.";
                    return OperatorResult::error(x + str);
                }
                ji+=1; //increment 4 times before incrementing j
                if (ji >= 4) {
                    j++;
                    ji = 0;
                }
                
                setiToEnd = true;
            }
            i2++;
        }

    } else if (parameters.value("edit_type").toString() == "ascii") { //ascii

        int outputSize = inputContainerSize - (length * 8) + (newBitsInRangeLength * 8);
        outBits = QSharedPointer<BitArray>(new BitArray(outputSize));
        bits->copyBits(0, outBits.data(), 0, outputSize, BitArray::Copy);

        QByteArray nair = newBitsInRange.toLocal8Bit();
        const char * newAsciiInRange = nair.data();

        if (newBitsInRangeLength == length) {     
            outBits->setBytes(start, newAsciiInRange, 0, newBitsInRangeLength);
        } else {
            QSharedPointer<BitArray> postBits = QSharedPointer<BitArray>(new BitArray(outputSize - start + length ));
            QByteArray byteArray = bits->readBytes(start + length, outputSize - 1);
            int endLength = byteArray.length();
            const char* data = byteArray.data();
            outBits->setBytes(start+newBitsInRangeLength, data, 0, endLength);
            outBits->setBytes(start, newAsciiInRange, 0, newBitsInRangeLength);
        }
    }

    //for each bit in the bit container
    //if bit index is less than the start index, or greater than the end index, copy the bit exactly

    auto bitContainer = BitContainer::create(outBits);

    outputContainers.append(bitContainer);
    //return OperatorResult::error("Hej");
    return OperatorResult::result(outputContainers, parameters);
    // TODO: Perform operation and return result with OperatorResult::result
    //return OperatorResult::error("Bits from start to length would be changed according to your entry, but that is not implemented yet.");
}