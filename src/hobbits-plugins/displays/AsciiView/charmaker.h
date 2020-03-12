#ifndef CHARMAKER_H
#define CHARMAKER_H

#include "frame.h"
#include <QString>

class CharMaker {
public:
    virtual ~CharMaker()
    {
    }
    virtual QString getName() = 0;
    virtual QString getDisplayChars(Frame frame, int offset) = 0;
};

#endif // CHARMAKER_H
