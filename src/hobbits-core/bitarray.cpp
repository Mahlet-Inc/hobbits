#include "bitarray.h"

#include <QRegularExpression>
#include <QString>

static char BIT_MASKS[8] = {
    -128, 0x40, 0x20, 0x10, 0x08, 0x04, 0x02, 0x01
};

static char INVERSE_BIT_MASKS[8] = {
    0x7f, -65, -33, -17, -9, -5, -3, -2
};

BitArray::BitArray() :
    m_size(0)
{

}

BitArray::BitArray(QByteArray bytes, int size) :
    m_bytes(bytes),
    m_size(size)
{
    Q_ASSERT_X(size >= 0, "Bit Array", QStringLiteral("Negative bit array size is not allowed").toLocal8Bit().data());
    Q_ASSERT_X(
            size <= bytes.size() * 8,
            "Bit Array",
            QStringLiteral("Bit array size %1 is too big for the given byte array").arg(size).toLocal8Bit().data());
}

BitArray::BitArray(const BitArray &other) :
    BitArray(other.getBytes(), other.size())
{
}

bool BitArray::at(int i) const
{
    Q_ASSERT_X(
            i < m_size,
            "Bit Array",
            QStringLiteral("Trying to access bit %1 of BitArray with size %2 bits").arg(i).arg(
                    m_size).toLocal8Bit().data());
    return m_bytes.at(i / 8) & BIT_MASKS[i % 8];
}

int BitArray::size() const
{
    return m_size;
}

void BitArray::set(int i, bool value)
{
    if (value) {
        m_bytes[i / 8] = m_bytes.at(i / 8) | BIT_MASKS[i % 8];
    }
    else {
        m_bytes[i / 8] = m_bytes.at(i / 8) & INVERSE_BIT_MASKS[i % 8];
    }
}

void BitArray::setSize(int size)
{
    m_size = size;
}

QByteArray BitArray::getBytes() const
{
    return m_bytes;
}

BitArray BitArray::xorAt(int offset, const BitArray &mask)
{
    BitArray result(QByteArray(((mask.size() - 1) / 8) + 1, 0x00), mask.size());
    for (int i = 0; i < mask.size(); i++) {
        result.set(i, at(offset + i) ^ mask.at(i));
    }
    return result;
}

BitArray BitArray::andAt(int offset, const BitArray &mask)
{
    BitArray result(QByteArray(((mask.size() - 1) / 8) + 1, 0x00), mask.size());
    for (int i = 0; i < mask.size(); i++) {
        result.set(i, at(offset + i) & mask.at(i));
    }
    return result;
}

BitArray BitArray::orAt(int offset, const BitArray &mask) const
{
    BitArray result(QByteArray(((mask.size() - 1) / 8) + 1, 0x00), mask.size());
    for (int i = 0; i < mask.size(); i++) {
        result.set(i, at(offset + i) | mask.at(i));
    }
    return result;
}

int BitArray::countOnes()
{
    int ones = 0;
    for (int i = 0; i < m_size; i++) {
        ones += at(i);
    }
    return ones;
}

QString BitArray::toString()
{
    QString binString = "";
    for (int i = 0; i < m_size; i++) {
        binString += at(i) ? "1" : "0";
    }
    return binString;
}

BitArray BitArray::fromString(QString bitArraySpec, QStringList parseErrors)
{
    int size = 0;
    if (bitArraySpec.startsWith("0x")) {
        QRegularExpression hexMatcher("^[0-9A-F]+$", QRegularExpression::CaseInsensitiveOption);
        QRegularExpressionMatch match = hexMatcher.match(bitArraySpec.mid(2));
        if (match.hasMatch()) {
            QString evenSpec = bitArraySpec.mid(2);
            if (evenSpec.size() % 2 == 1) {
                evenSpec += "0";
            }
            QByteArray bytes = QByteArray::fromHex(evenSpec.toLocal8Bit());
            size = qMin((bitArraySpec.length() - 2) * 4, bytes.size() * 8);
            return BitArray(bytes, size);
        }
        else {
            parseErrors.append(QString("Expected only hex digits in '0x'-prefixed data - got '%1'").arg(bitArraySpec));
            return BitArray();
        }
    }
    else if (bitArraySpec.startsWith("0o")) {
        size = (bitArraySpec.length() - 2) * 3;
        QByteArray bytes(size / 8 + 1, 0x00);
        BitArray bits = BitArray(bytes, size);
        bool parseOk;
        for (int i = 2; i < bitArraySpec.size(); i++) {
            parseOk = true;
            int val = bitArraySpec.mid(i, 1).toInt(&parseOk, 8);

            if (!parseOk) {
                parseErrors.append(
                        QString("Expected octal digit in '0o'-prefixed data - got %1").arg(
                                bitArraySpec.at(
                                        i)));
                continue;
            }

            for (int bit = 0; bit < 3; bit++) {
                bits.set((i - 2) * 3 + bit, val & BIT_MASKS[5 + bit]);
            }
        }
        return bits;
    }
    else if (bitArraySpec.startsWith("0b")) {
        size = bitArraySpec.length() - 2;
        QByteArray bytes(size / 8 + 1, 0x00);
        BitArray bits = BitArray(bytes, size);
        for (int i = 2; i < bitArraySpec.size(); i++) {
            if (bitArraySpec.at(i) == '1') {
                bits.set(i - 2, true);
            }
            else if (bitArraySpec.at(i) != '0') {
                parseErrors.append(
                        QString("Expected '1' or '0' in '0b'-prefixed data - got '%1'").arg(
                                bitArraySpec.at(
                                        i)));
            }
        }
        return bits;
    }
    else {
        QByteArray bytes = bitArraySpec.toLocal8Bit();
        size = bitArraySpec.length() * 8;
        return BitArray(bytes, size);
    }
}
