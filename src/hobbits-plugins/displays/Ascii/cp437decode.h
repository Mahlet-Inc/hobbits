#ifndef CP437DECODE_H
#define CP437DECODE_H

#include "frame.h"
#include <QMap>

static QMap<char, QChar> UTF8_CP437_MAP = {
    { char(0x00), QChar(0x002e) }, //NULL print as period/full stop
    { char(0x01), QChar(0x263a) },
    { char(0x02), QChar(0x263b) },
    { char(0x03), QChar(0x2665) },
    { char(0x04), QChar(0x2666) },
    { char(0x05), QChar(0x2663) },
    { char(0x06), QChar(0x2660) },
    { char(0x07), QChar(0x2022) },
    { char(0x08), QChar(0x25d8) },
    { char(0x09), QChar(0x25cb) },
    { char(0x0a), QChar(0x25d9) },
    { char(0x0b), QChar(0x2642) },
    { char(0x0c), QChar(0x2640) },
    { char(0x0d), QChar(0x266a) },
    { char(0x0e), QChar(0x266b) },
    { char(0x0f), QChar(0x263c) },
    { char(0x10), QChar(0x25ba) },
    { char(0x11), QChar(0x25c4) },
    { char(0x12), QChar(0x2195) },
    { char(0x13), QChar(0x203c) },
    { char(0x14), QChar(0x00b6) },
    { char(0x15), QChar(0x00a7) },
    { char(0x16), QChar(0x25ac) },
    { char(0x17), QChar(0x21a8) },
    { char(0x18), QChar(0x2191) },
    { char(0x19), QChar(0x2193) },
    { char(0x1a), QChar(0x2192) },
    { char(0x1b), QChar(0x2190) },
    { char(0x1c), QChar(0x221f) },
    { char(0x1d), QChar(0x2194) },
    { char(0x1e), QChar(0x25b2) },
    { char(0x1f), QChar(0x25bc) },
    { char(0x20), QChar(0x0020) }, //SPACE
    { char(0x21), QChar(0x0021) }, //EXCLAMATION MARK
    { char(0x22), QChar(0x0022) }, //QUOTATION MARK
    { char(0x23), QChar(0x0023) }, //NUMBER SIGN
    { char(0x24), QChar(0x0024) }, //DOLLAR SIGN
    { char(0x25), QChar(0x0025) }, //PERCENT SIGN
    { char(0x26), QChar(0x0026) }, //AMPERSAND
    { char(0x27), QChar(0x0027) }, //APOSTROPHE
    { char(0x28), QChar(0x0028) }, //LEFT PARENTHESIS
    { char(0x29), QChar(0x0029) }, //RIGHT PARENTHESIS
    { char(0x2a), QChar(0x002a) }, //ASTERISK
    { char(0x2b), QChar(0x002b) }, //PLUS SIGN
    { char(0x2c), QChar(0x002c) }, //COMMA
    { char(0x2d), QChar(0x002d) }, //HYPHEN-MINUS
    { char(0x2e), QChar(0x002e) }, //FULL STOP
    { char(0x2f), QChar(0x002f) }, //SOLIDUS
    { char(0x30), QChar(0x0030) }, //DIGIT ZERO
    { char(0x31), QChar(0x0031) }, //DIGIT ONE
    { char(0x32), QChar(0x0032) }, //DIGIT TWO
    { char(0x33), QChar(0x0033) }, //DIGIT THREE
    { char(0x34), QChar(0x0034) }, //DIGIT FOUR
    { char(0x35), QChar(0x0035) }, //DIGIT FIVE
    { char(0x36), QChar(0x0036) }, //DIGIT SIX
    { char(0x37), QChar(0x0037) }, //DIGIT SEVEN
    { char(0x38), QChar(0x0038) }, //DIGIT EIGHT
    { char(0x39), QChar(0x0039) }, //DIGIT NINE
    { char(0x3a), QChar(0x003a) }, //COLON
    { char(0x3b), QChar(0x003b) }, //SEMICOLON
    { char(0x3c), QChar(0x003c) }, //LESS-THAN SIGN
    { char(0x3d), QChar(0x003d) }, //EQUALS SIGN
    { char(0x3e), QChar(0x003e) }, //GREATER-THAN SIGN
    { char(0x3f), QChar(0x003f) }, //QUESTION MARK
    { char(0x40), QChar(0x0040) }, //COMMERCIAL AT
    { char(0x41), QChar(0x0041) }, //LATIN CAPITAL LETTER A
    { char(0x42), QChar(0x0042) }, //LATIN CAPITAL LETTER B
    { char(0x43), QChar(0x0043) }, //LATIN CAPITAL LETTER C
    { char(0x44), QChar(0x0044) }, //LATIN CAPITAL LETTER D
    { char(0x45), QChar(0x0045) }, //LATIN CAPITAL LETTER E
    { char(0x46), QChar(0x0046) }, //LATIN CAPITAL LETTER F
    { char(0x47), QChar(0x0047) }, //LATIN CAPITAL LETTER G
    { char(0x48), QChar(0x0048) }, //LATIN CAPITAL LETTER H
    { char(0x49), QChar(0x0049) }, //LATIN CAPITAL LETTER I
    { char(0x4a), QChar(0x004a) }, //LATIN CAPITAL LETTER J
    { char(0x4b), QChar(0x004b) }, //LATIN CAPITAL LETTER K
    { char(0x4c), QChar(0x004c) }, //LATIN CAPITAL LETTER L
    { char(0x4d), QChar(0x004d) }, //LATIN CAPITAL LETTER M
    { char(0x4e), QChar(0x004e) }, //LATIN CAPITAL LETTER N
    { char(0x4f), QChar(0x004f) }, //LATIN CAPITAL LETTER O
    { char(0x50), QChar(0x0050) }, //LATIN CAPITAL LETTER P
    { char(0x51), QChar(0x0051) }, //LATIN CAPITAL LETTER Q
    { char(0x52), QChar(0x0052) }, //LATIN CAPITAL LETTER R
    { char(0x53), QChar(0x0053) }, //LATIN CAPITAL LETTER S
    { char(0x54), QChar(0x0054) }, //LATIN CAPITAL LETTER T
    { char(0x55), QChar(0x0055) }, //LATIN CAPITAL LETTER U
    { char(0x56), QChar(0x0056) }, //LATIN CAPITAL LETTER V
    { char(0x57), QChar(0x0057) }, //LATIN CAPITAL LETTER W
    { char(0x58), QChar(0x0058) }, //LATIN CAPITAL LETTER X
    { char(0x59), QChar(0x0059) }, //LATIN CAPITAL LETTER Y
    { char(0x5a), QChar(0x005a) }, //LATIN CAPITAL LETTER Z
    { char(0x5b), QChar(0x005b) }, //LEFT SQUARE BRACKET
    { char(0x5c), QChar(0x005c) }, //REVERSE SOLIDUS
    { char(0x5d), QChar(0x005d) }, //RIGHT SQUARE BRACKET
    { char(0x5e), QChar(0x005e) }, //CIRCUMFLEX ACCENT
    { char(0x5f), QChar(0x005f) }, //LOW LINE
    { char(0x60), QChar(0x0060) }, //GRAVE ACCENT
    { char(0x61), QChar(0x0061) }, //LATIN SMALL LETTER A
    { char(0x62), QChar(0x0062) }, //LATIN SMALL LETTER B
    { char(0x63), QChar(0x0063) }, //LATIN SMALL LETTER C
    { char(0x64), QChar(0x0064) }, //LATIN SMALL LETTER D
    { char(0x65), QChar(0x0065) }, //LATIN SMALL LETTER E
    { char(0x66), QChar(0x0066) }, //LATIN SMALL LETTER F
    { char(0x67), QChar(0x0067) }, //LATIN SMALL LETTER G
    { char(0x68), QChar(0x0068) }, //LATIN SMALL LETTER H
    { char(0x69), QChar(0x0069) }, //LATIN SMALL LETTER I
    { char(0x6a), QChar(0x006a) }, //LATIN SMALL LETTER J
    { char(0x6b), QChar(0x006b) }, //LATIN SMALL LETTER K
    { char(0x6c), QChar(0x006c) }, //LATIN SMALL LETTER L
    { char(0x6d), QChar(0x006d) }, //LATIN SMALL LETTER M
    { char(0x6e), QChar(0x006e) }, //LATIN SMALL LETTER N
    { char(0x6f), QChar(0x006f) }, //LATIN SMALL LETTER O
    { char(0x70), QChar(0x0070) }, //LATIN SMALL LETTER P
    { char(0x71), QChar(0x0071) }, //LATIN SMALL LETTER Q
    { char(0x72), QChar(0x0072) }, //LATIN SMALL LETTER R
    { char(0x73), QChar(0x0073) }, //LATIN SMALL LETTER S
    { char(0x74), QChar(0x0074) }, //LATIN SMALL LETTER T
    { char(0x75), QChar(0x0075) }, //LATIN SMALL LETTER U
    { char(0x76), QChar(0x0076) }, //LATIN SMALL LETTER V
    { char(0x77), QChar(0x0077) }, //LATIN SMALL LETTER W
    { char(0x78), QChar(0x0078) }, //LATIN SMALL LETTER X
    { char(0x79), QChar(0x0079) }, //LATIN SMALL LETTER Y
    { char(0x7a), QChar(0x007a) }, //LATIN SMALL LETTER Z
    { char(0x7b), QChar(0x007b) }, //LEFT CURLY BRACKET
    { char(0x7c), QChar(0x007c) }, //VERTICAL LINE
    { char(0x7d), QChar(0x007d) }, //RIGHT CURLY BRACKET
    { char(0x7e), QChar(0x007e) }, //TILDE
    { char(0x7f), QChar(0x2302) }, //HOUSE
    { char(0x80), QChar(0x00c7) }, //LATIN CAPITAL LETTER C WITH CEDILLA
    { char(0x81), QChar(0x00fc) }, //LATIN SMALL LETTER U WITH DIAERESIS
    { char(0x82), QChar(0x00e9) }, //LATIN SMALL LETTER E WITH ACUTE
    { char(0x83), QChar(0x00e2) }, //LATIN SMALL LETTER A WITH CIRCUMFLEX
    { char(0x84), QChar(0x00e4) }, //LATIN SMALL LETTER A WITH DIAERESIS
    { char(0x85), QChar(0x00e0) }, //LATIN SMALL LETTER A WITH GRAVE
    { char(0x86), QChar(0x00e5) }, //LATIN SMALL LETTER A WITH RING ABOVE
    { char(0x87), QChar(0x00e7) }, //LATIN SMALL LETTER C WITH CEDILLA
    { char(0x88), QChar(0x00ea) }, //LATIN SMALL LETTER E WITH CIRCUMFLEX
    { char(0x89), QChar(0x00eb) }, //LATIN SMALL LETTER E WITH DIAERESIS
    { char(0x8a), QChar(0x00e8) }, //LATIN SMALL LETTER E WITH GRAVE
    { char(0x8b), QChar(0x00ef) }, //LATIN SMALL LETTER I WITH DIAERESIS
    { char(0x8c), QChar(0x00ee) }, //LATIN SMALL LETTER I WITH CIRCUMFLEX
    { char(0x8d), QChar(0x00ec) }, //LATIN SMALL LETTER I WITH GRAVE
    { char(0x8e), QChar(0x00c4) }, //LATIN CAPITAL LETTER A WITH DIAERESIS
    { char(0x8f), QChar(0x00c5) }, //LATIN CAPITAL LETTER A WITH RING ABOVE
    { char(0x90), QChar(0x00c9) }, //LATIN CAPITAL LETTER E WITH ACUTE
    { char(0x91), QChar(0x00e6) }, //LATIN SMALL LIGATURE AE
    { char(0x92), QChar(0x00c6) }, //LATIN CAPITAL LIGATURE AE
    { char(0x93), QChar(0x00f4) }, //LATIN SMALL LETTER O WITH CIRCUMFLEX
    { char(0x94), QChar(0x00f6) }, //LATIN SMALL LETTER O WITH DIAERESIS
    { char(0x95), QChar(0x00f2) }, //LATIN SMALL LETTER O WITH GRAVE
    { char(0x96), QChar(0x00fb) }, //LATIN SMALL LETTER U WITH CIRCUMFLEX
    { char(0x97), QChar(0x00f9) }, //LATIN SMALL LETTER U WITH GRAVE
    { char(0x98), QChar(0x00ff) }, //LATIN SMALL LETTER Y WITH DIAERESIS
    { char(0x99), QChar(0x00d6) }, //LATIN CAPITAL LETTER O WITH DIAERESIS
    { char(0x9a), QChar(0x00dc) }, //LATIN CAPITAL LETTER U WITH DIAERESIS
    { char(0x9b), QChar(0x00a2) }, //CENT SIGN
    { char(0x9c), QChar(0x00a3) }, //POUND SIGN
    { char(0x9d), QChar(0x00a5) }, //YEN SIGN
    { char(0x9e), QChar(0x20a7) }, //PESETA SIGN
    { char(0x9f), QChar(0x0192) }, //LATIN SMALL LETTER F WITH HOOK
    { char(0xa0), QChar(0x00e1) }, //LATIN SMALL LETTER A WITH ACUTE
    { char(0xa1), QChar(0x00ed) }, //LATIN SMALL LETTER I WITH ACUTE
    { char(0xa2), QChar(0x00f3) }, //LATIN SMALL LETTER O WITH ACUTE
    { char(0xa3), QChar(0x00fa) }, //LATIN SMALL LETTER U WITH ACUTE
    { char(0xa4), QChar(0x00f1) }, //LATIN SMALL LETTER N WITH TILDE
    { char(0xa5), QChar(0x00d1) }, //LATIN CAPITAL LETTER N WITH TILDE
    { char(0xa6), QChar(0x00aa) }, //FEMININE ORDINAL INDICATOR
    { char(0xa7), QChar(0x00ba) }, //MASCULINE ORDINAL INDICATOR
    { char(0xa8), QChar(0x00bf) }, //INVERTED QUESTION MARK
    { char(0xa9), QChar(0x2310) }, //REVERSED NOT SIGN
    { char(0xaa), QChar(0x00ac) }, //NOT SIGN
    { char(0xab), QChar(0x00bd) }, //VULGAR FRACTION ONE HALF
    { char(0xac), QChar(0x00bc) }, //VULGAR FRACTION ONE QUARTER
    { char(0xad), QChar(0x00a1) }, //INVERTED EXCLAMATION MARK
    { char(0xae), QChar(0x00ab) }, //LEFT-POINTING DOUBLE ANGLE QUOTATION MARK
    { char(0xaf), QChar(0x00bb) }, //RIGHT-POINTING DOUBLE ANGLE QUOTATION MARK
    { char(0xb0), QChar(0x2591) }, //LIGHT SHADE
    { char(0xb1), QChar(0x2592) }, //MEDIUM SHADE
    { char(0xb2), QChar(0x2593) }, //DARK SHADE
    { char(0xb3), QChar(0x2502) }, //BOX DRAWINGS LIGHT VERTICAL
    { char(0xb4), QChar(0x2524) }, //BOX DRAWINGS LIGHT VERTICAL AND LEFT
    { char(0xb5), QChar(0x2561) }, //BOX DRAWINGS VERTICAL SINGLE AND LEFT DOUBLE
    { char(0xb6), QChar(0x2562) }, //BOX DRAWINGS VERTICAL DOUBLE AND LEFT SINGLE
    { char(0xb7), QChar(0x2556) }, //BOX DRAWINGS DOWN DOUBLE AND LEFT SINGLE
    { char(0xb8), QChar(0x2555) }, //BOX DRAWINGS DOWN SINGLE AND LEFT DOUBLE
    { char(0xb9), QChar(0x2563) }, //BOX DRAWINGS DOUBLE VERTICAL AND LEFT
    { char(0xba), QChar(0x2551) }, //BOX DRAWINGS DOUBLE VERTICAL
    { char(0xbb), QChar(0x2557) }, //BOX DRAWINGS DOUBLE DOWN AND LEFT
    { char(0xbc), QChar(0x255d) }, //BOX DRAWINGS DOUBLE UP AND LEFT
    { char(0xbd), QChar(0x255c) }, //BOX DRAWINGS UP DOUBLE AND LEFT SINGLE
    { char(0xbe), QChar(0x255b) }, //BOX DRAWINGS UP SINGLE AND LEFT DOUBLE
    { char(0xbf), QChar(0x2510) }, //BOX DRAWINGS LIGHT DOWN AND LEFT
    { char(0xc0), QChar(0x2514) }, //BOX DRAWINGS LIGHT UP AND RIGHT
    { char(0xc1), QChar(0x2534) }, //BOX DRAWINGS LIGHT UP AND HORIZONTAL
    { char(0xc2), QChar(0x252c) }, //BOX DRAWINGS LIGHT DOWN AND HORIZONTAL
    { char(0xc3), QChar(0x251c) }, //BOX DRAWINGS LIGHT VERTICAL AND RIGHT
    { char(0xc4), QChar(0x2500) }, //BOX DRAWINGS LIGHT HORIZONTAL
    { char(0xc5), QChar(0x253c) }, //BOX DRAWINGS LIGHT VERTICAL AND HORIZONTAL
    { char(0xc6), QChar(0x255e) }, //BOX DRAWINGS VERTICAL SINGLE AND RIGHT DOUBLE
    { char(0xc7), QChar(0x255f) }, //BOX DRAWINGS VERTICAL DOUBLE AND RIGHT SINGLE
    { char(0xc8), QChar(0x255a) }, //BOX DRAWINGS DOUBLE UP AND RIGHT
    { char(0xc9), QChar(0x2554) }, //BOX DRAWINGS DOUBLE DOWN AND RIGHT
    { char(0xca), QChar(0x2569) }, //BOX DRAWINGS DOUBLE UP AND HORIZONTAL
    { char(0xcb), QChar(0x2566) }, //BOX DRAWINGS DOUBLE DOWN AND HORIZONTAL
    { char(0xcc), QChar(0x2560) }, //BOX DRAWINGS DOUBLE VERTICAL AND RIGHT
    { char(0xcd), QChar(0x2550) }, //BOX DRAWINGS DOUBLE HORIZONTAL
    { char(0xce), QChar(0x256c) }, //BOX DRAWINGS DOUBLE VERTICAL AND HORIZONTAL
    { char(0xcf), QChar(0x2567) }, //BOX DRAWINGS UP SINGLE AND HORIZONTAL DOUBLE
    { char(0xd0), QChar(0x2568) }, //BOX DRAWINGS UP DOUBLE AND HORIZONTAL SINGLE
    { char(0xd1), QChar(0x2564) }, //BOX DRAWINGS DOWN SINGLE AND HORIZONTAL DOUBLE
    { char(0xd2), QChar(0x2565) }, //BOX DRAWINGS DOWN DOUBLE AND HORIZONTAL SINGLE
    { char(0xd3), QChar(0x2559) }, //BOX DRAWINGS UP DOUBLE AND RIGHT SINGLE
    { char(0xd4), QChar(0x2558) }, //BOX DRAWINGS UP SINGLE AND RIGHT DOUBLE
    { char(0xd5), QChar(0x2552) }, //BOX DRAWINGS DOWN SINGLE AND RIGHT DOUBLE
    { char(0xd6), QChar(0x2553) }, //BOX DRAWINGS DOWN DOUBLE AND RIGHT SINGLE
    { char(0xd7), QChar(0x256b) }, //BOX DRAWINGS VERTICAL DOUBLE AND HORIZONTAL SINGLE
    { char(0xd8), QChar(0x256a) }, //BOX DRAWINGS VERTICAL SINGLE AND HORIZONTAL DOUBLE
    { char(0xd9), QChar(0x2518) }, //BOX DRAWINGS LIGHT UP AND LEFT
    { char(0xda), QChar(0x250c) }, //BOX DRAWINGS LIGHT DOWN AND RIGHT
    { char(0xdb), QChar(0x2588) }, //FULL BLOCK
    { char(0xdc), QChar(0x2584) }, //LOWER HALF BLOCK
    { char(0xdd), QChar(0x258c) }, //LEFT HALF BLOCK
    { char(0xde), QChar(0x2590) }, //RIGHT HALF BLOCK
    { char(0xdf), QChar(0x2580) }, //UPPER HALF BLOCK
    { char(0xe0), QChar(0x03b1) }, //GREEK SMALL LETTER ALPHA
    { char(0xe1), QChar(0x00df) }, //LATIN SMALL LETTER SHARP S
    { char(0xe2), QChar(0x0393) }, //GREEK CAPITAL LETTER GAMMA
    { char(0xe3), QChar(0x03c0) }, //GREEK SMALL LETTER PI
    { char(0xe4), QChar(0x03a3) }, //GREEK CAPITAL LETTER SIGMA
    { char(0xe5), QChar(0x03c3) }, //GREEK SMALL LETTER SIGMA
    { char(0xe6), QChar(0x00b5) }, //MICRO SIGN
    { char(0xe7), QChar(0x03c4) }, //GREEK SMALL LETTER TAU
    { char(0xe8), QChar(0x03a6) }, //GREEK CAPITAL LETTER PHI
    { char(0xe9), QChar(0x0398) }, //GREEK CAPITAL LETTER THETA
    { char(0xea), QChar(0x03a9) }, //GREEK CAPITAL LETTER OMEGA
    { char(0xeb), QChar(0x03b4) }, //GREEK SMALL LETTER DELTA
    { char(0xec), QChar(0x221e) }, //INFINITY
    { char(0xed), QChar(0x03c6) }, //GREEK SMALL LETTER PHI
    { char(0xee), QChar(0x03b5) }, //GREEK SMALL LETTER EPSILON
    { char(0xef), QChar(0x2229) }, //INTERSECTION
    { char(0xf0), QChar(0x2261) }, //IDENTICAL TO
    { char(0xf1), QChar(0x00b1) }, //PLUS-MINUS SIGN
    { char(0xf2), QChar(0x2265) }, //GREATER-THAN OR EQUAL TO
    { char(0xf3), QChar(0x2264) }, //LESS-THAN OR EQUAL TO
    { char(0xf4), QChar(0x2320) }, //TOP HALF INTEGRAL
    { char(0xf5), QChar(0x2321) }, //BOTTOM HALF INTEGRAL
    { char(0xf6), QChar(0x00f7) }, //DIVISION SIGN
    { char(0xf7), QChar(0x2248) }, //ALMOST EQUAL TO
    { char(0xf8), QChar(0x00b0) }, //DEGREE SIGN
    { char(0xf9), QChar(0x2219) }, //BULLET OPERATOR
    { char(0xfa), QChar(0x00b7) }, //MIDDLE DOT
    { char(0xfb), QChar(0x221a) }, //SQUARE ROOT
    { char(0xfc), QChar(0x207f) }, //SUPERSCRIPT LATIN SMALL LETTER N
    { char(0xfd), QChar(0x00b2) }, //SUPERSCRIPT TWO
    { char(0xfe), QChar(0x25a0) }, //BLACK SQUARE
    { char(0xff), QChar(0x0020) }  //NO-BREAK SPACE print as space
};

QString decodeCp437Bits(const Frame& f, qint64 &bitOffset) {
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
        frameString += UTF8_CP437_MAP.value(byte);
    }
    bitOffset += 8;
    return frameString;
}

#endif // CP437DECODE_H
