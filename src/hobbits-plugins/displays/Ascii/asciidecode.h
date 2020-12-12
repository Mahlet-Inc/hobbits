#ifndef ASCIIDECODE_H
#define ASCIIDECODE_H

#include "frame.h"

QString decodeAsciiBits(const Frame& f, qint64 &bitOffset) {
    QString frameString = "";
    if (bitOffset + 7 >= f.size()) {
        // partial char
        frameString += '.';
    }
    else {
        char byte = 0;
        for (int bit = 0; bit < 8; bit++) {
            byte <<= 1;
            if (f.at(bitOffset + bit)) {
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
    bitOffset += 8;
    return frameString;
}

#endif // ASCIIDECODE_H
