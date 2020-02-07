#ifndef MATHPARSER_H
#define MATHPARSER_H
#include "parseresult.h"
#include <QObject>

enum ParseType {
    Default = 0,
    No_Unit = 1,
    Hex = 2,
    Bin = 3,
    Dec = 4,
    Oct = 5,
    No_Math = 6
};

class MathParser
{
public:
    MathParser();

    QString tokens;
    int pos;
    QChar tok();
    void next();
    void prev();
    ParseResult parseInput(QString tokens, ParseType t, int start = 0);
    ParseResult parseInput(QString tokens);

private:
    ParseResult base();     // value, multiplier
    ParseResult factors();
    int unit();     // return multiplier
    ParseResult factor(bool);
    QChar op();
    ParseResult whole(bool);
    ParseResult wholehex(bool);
    ParseResult wholedec(bool);
    ParseResult wholeoct(bool);
    ParseResult wholebin(bool);
    QChar hex();
    QChar dec();
    QChar oct();
    QChar bin();

};
#endif // MATHPARSER_H
