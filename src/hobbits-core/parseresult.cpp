#include "parseresult.h"

ParseResult::ParseResult()
{
}

ParseResult::ParseResult(qlonglong val, int mult)
{
    this->_val = val;
    this->_mult = mult;
}

qlonglong ParseResult::getVal()
{
    return this->_val;
}

int ParseResult::getMult()
{
    return this->_mult;
}

qlonglong ParseResult::getResult()
{
    if (this->isValid()) {
        return this->_val * this->_mult;
    }
    return -1;
}

bool ParseResult::isValid()
{
    return (this->_mult >= 0 && this->_val >= 0); // No negative results allowed either.
}
