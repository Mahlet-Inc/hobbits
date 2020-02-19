#ifndef BITARRAY_H
#define BITARRAY_H

#include <QByteArray>
#include <QStringList>

#include "hobbits-core_global.h"

class HOBBITSCORESHARED_EXPORT BitArray
{
public:
    BitArray();
    BitArray(QByteArray bytes, int size);
    BitArray(const BitArray &other);

    bool at(int i) const;
    int size() const;

    static BitArray fromString(QString bitArraySpec, QStringList parseErrors = QStringList());

    void set(int i, bool value);
    void setSize(int size);

    BitArray xorAt(int offset, const BitArray &mask);
    BitArray andAt(int offset, const BitArray &mask);
    BitArray orAt(int offset, const BitArray &mask) const;

    QByteArray getBytes() const;

    QString toString();

    int countOnes();

private:
    QByteArray m_bytes;
    int m_size;

};

inline bool operator==(const BitArray &b1, const BitArray &b2)
{
    return b1.getBytes() == b2.getBytes();
}

inline uint qHash(const BitArray &key, uint seed)
{
    return qHash(key.getBytes(), seed) ^ uint(key.size());
}

#endif // BITARRAY_H
