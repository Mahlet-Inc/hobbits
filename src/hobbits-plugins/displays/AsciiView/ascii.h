#ifndef ASCII_H
#define ASCII_H

#include "charmaker.h"

class Ascii : public CharMaker
{
public:
    Ascii();
    QString getName() override;
    QString getDisplayChars(Frame frame, int offset) override;
};

#endif // ASCII_H
