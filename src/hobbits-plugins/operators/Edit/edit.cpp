#include "edit.h"
#include "editeditor.h"

Edit::Edit()
{
    QList<ParameterDelegate::ParameterInfo> infos = {
        // TODO: add parameters like {"myparametername", ParameterDelegate::ParameterType::Decimal}
        {"start", ParameterDelegate::ParameterType::Decimal},
        {"length", ParameterDelegate::ParameterType::Decimal},
        {"new_bits_in_range", ParameterDelegate::ParameterType::String, false}
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
    return "Load Hexidecimal characters, and modify them individually.";
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
    if (inputContainers.length() != 1) {
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

    //length = 8; //force for now

    //TODO: pte_bits is already filled with data from start to length
    //
    //auto bitContainer = inputContainers.first();
    QList<QSharedPointer<BitContainer>> outputContainers;

    //things I can do:
    // create an identical bit container
    // create a bitContainer of same size with all zeros
    // get bit array from inputContainer

    //things I'd like to do:
    //change individual bits in BitArray
    

    //bits as ?
    int end = start + length;
    int inputContainerSize = inputContainers.at(0)->size();
    QSharedPointer<const BitArray> bits = inputContainers.at(0)->bits();
    int outputSize = inputContainerSize - length + newBitsInRange.length();
    auto outBits = QSharedPointer<BitArray>(new BitArray(outputSize));
    bits->copyBits(0, outBits.data(), 0, outputSize, BitArray::Copy);

    //do something with bits here
    //ex 0000 1111 -> 1111 0000
    
    int j = 0; //j represents the index of the newBitsInRange string
    int i2 = 0; //the index from the input container
    bool setiToEnd;

    for (int i = 0; i < outputSize; i ++) { //i represents the index of the outBits, soon to be outputContainer
        if (i < start || i > start + newBitsInRange.length() - 1) {
            if (setiToEnd) {
                i2 = end;
                setiToEnd = false;
            }
            if (i2 < inputContainerSize) {
                outBits->set(i, inputContainers.at(0)->bits()->at(i2)); //copy bits from input container
            }
            i2+=1;
        } else {
            const QChar x = newBitsInRange.at(j);
            if (x.digitValue() == 0)
                outBits->set(i, false); //false is 0, true is 1
            else
                outBits->set(i, true); //false is 0, true is 1
            j += 1;
            setiToEnd = true;
        }
    }

    //for each bit in the bit container
    //if bit index is less than the start index, or greater than the end index, copy the bit exactly

    auto bitContainer = BitContainer::create(outBits);

    outputContainers.append(bitContainer);
    return OperatorResult::result(outputContainers, parameters);
    // TODO: Perform operation and return result with OperatorResult::result
    //return OperatorResult::error("Bits from start to length would be changed according to your entry, but that is not implemented yet.");
}
