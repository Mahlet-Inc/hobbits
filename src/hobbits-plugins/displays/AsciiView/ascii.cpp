#include "ascii.h"

Ascii::Ascii()
{

}

QString Ascii::getName() {
    return "ASCII";
}

QString Ascii::getDisplayChars(Frame frame, int offset) {
    QString frameString = "";
    if (offset + 7 >= frame.size()) {
        // partial char
        frameString += '.';
    }
    else {
        char byte = 0;
        for (int bit = 0; bit < 8; bit++) {
            byte <<= 1;
            if (frame.at(offset + bit)) {
                byte |= 0x01;
            }
        }
        if (isprint(byte)) {
            frameString += byte;
        }
        else {
            frameString += '.';
        }
    }
    return frameString;
}
