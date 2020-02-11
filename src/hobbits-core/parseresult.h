#ifndef PARSERESULT_H
#define PARSERESULT_H
#include <QObject>

#include "hobbits-core_global.h"

class HOBBITSCORESHARED_EXPORT ParseResult
{
public:
    ParseResult();
    ParseResult(qlonglong, int);

    qlonglong getVal(); // Returns evaluated value of math expression
    int getMult(); // Returns unit multiplier
    qlonglong getResult(); // Returns total # of bits
    bool isValid(); // Checks if expression was valid

private:
    qlonglong _val;
    int _mult;
};

#endif // PARSERESULT_H
