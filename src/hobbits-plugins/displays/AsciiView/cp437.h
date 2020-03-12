#ifndef CP437_H
#define CP437_H

#include "charmaker.h"

class Cp437 : public CharMaker
{
public:
    Cp437();
    QString getName() override;
    QString getDisplayChars(Frame frame, int offset) override;
};

#endif // CP437_H
