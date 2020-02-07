#include "mathparser.h"
#include <QVector>

/*   |-------------------------------------------------|
 *   |   Input format: [expression][unit]              |
 *   |                                                 |
 *   |   Expression consists of base 2,8,10,16 numbers |
 *   | separated by operations (+,-,*,/).              |
 *   |                                                 |
 *   |   Units: ["Bits"] or "Bytes"                    |
 *   |                                                 |
 *   |   Hex digits should be lowercase                |
 *   |                                                 |
 *   |   Spaces ignored                                |
 *   |_________________________________________________|
 */


MathParser::MathParser()
{
}

QChar MathParser::tok()
{
    return (pos < this->tokens.length()) ? QChar(this->tokens[this->pos]) : *(new QChar(';'));
}

void MathParser::next()
{
    this->pos++;
}

void MathParser::prev()
{
    this->pos--;
}

QChar MathParser::op()
{
    QString ops = "+-*/";
    if (ops.contains(this->tok())) {
        return this->tok();
    }
    return '\0';
}

QChar MathParser::bin()
{

    if (this->tok() == "0" || this->tok() == "1") {
        return this->tok();
    }
    return '\0';
}

QChar MathParser::oct()
{
    QString octs = "234567";
    if (!bin().isNull()) {
        return this->tok();
    }
    if (octs.contains(this->tok())) {
        return this->tok();
    }
    return '\0';
}

QChar MathParser::dec()
{
    QString decs = "89";
    if (!oct().isNull()) {
        return this->tok();
    }
    if (decs.contains(this->tok())) {
        return this->tok();
    }
    return '\0';
}

QChar MathParser::hex()
{
    QString hexs = "abcdef";
    if (!dec().isNull()) {
        return this->tok();
    }
    if (hexs.contains(this->tok(), Qt::CaseSensitive)) {
        return this->tok();
    }
    return '\0';
}

ParseResult MathParser::wholebin(bool move_back = true)
{
    QString binaryString = "";
    bool ok;
    if (this->tok() == "0") {
        this->next();
        if (this->tok() == "b") {
            this->next();
            if (!bin().isNull()) {
                binaryString += this->tok();
                this->next();
                while (!bin().isNull()) {
                    binaryString += this->tok();
                    this->next();
                }
                if (move_back) {
                    for (int i = 0; i < binaryString.length() + 2; i++) {
                        this->prev();
                    }
                }
                return ParseResult(binaryString.toInt(&ok, 2), 1);
            }
            else {
                this->prev();
                this->prev();
                return ParseResult(-1, -1);
            }
        }
        else {
            this->prev();
            return ParseResult(-1, -1);
        }
    }
    else {
        return ParseResult(-1, -1);
    }
}

ParseResult MathParser::wholehex(bool move_back = true)
{
    QString hexString = "";
    bool ok;
    if (this->tok() == "0") {
        this->next();
        if (this->tok() == "x") {
            this->next();
            if (!hex().isNull()) {
                hexString += this->tok();
                this->next();
                while (!hex().isNull()) {
                    hexString += this->tok();
                    this->next();
                }
                if (move_back) {
                    for (int i = 0; i < hexString.length() + 2; i++) {
                        this->prev();
                    }
                }
                return ParseResult(hexString.toLongLong(&ok, 16), 1);
            }
            else {
                this->prev();
                this->prev();
                return ParseResult(-1, -1);
            }
        }
        else {
            this->prev();
            return ParseResult(-1, -1);
        }
    }
    else {
        return ParseResult(-1, -1);
    }
}

ParseResult MathParser::wholeoct(bool move_back = true)
{
    QString octString = "";
    bool ok;
    if (this->tok() == "0") {
        this->next();
        if (this->tok() == "o") {
            this->next();
            if (!oct().isNull()) {
                octString += this->tok();
                this->next();
                while (!oct().isNull()) {
                    octString += this->tok();
                    this->next();
                }
                if (move_back) {
                    for (int i = 0; i < octString.length() + 2; i++) {
                        this->prev();
                    }
                }
                return ParseResult(octString.toLongLong(&ok, 8), 1);
            }
            else {
                this->prev();
                this->prev();
                return ParseResult(-1, -1);
            }
        }
        else {
            this->prev();
            return ParseResult(-1, -1);
        }
    }
    else {
        return ParseResult(-1, -1);
    }
}

ParseResult MathParser::wholedec(bool move_back = true)
{
    QString decString = "";
    if (!dec().isNull()) {
        decString += this->tok();
        this->next();
        while (!dec().isNull()) {
            decString += this->tok();
            this->next();
        }
        if (move_back) {
            for (int i = 0; i < decString.length(); i++) {
                this->prev();
            }
        }
        return ParseResult(decString.toLongLong(), 1);
    }
    return ParseResult(-1, -1);
}

ParseResult MathParser::whole(bool move_back = true)
{
    if (wholebin().isValid()) {
        return wholebin(move_back);
    }
    if (wholeoct().isValid()) {
        return wholeoct(move_back);
    }
    if (wholehex().isValid()) {
        return wholehex(move_back);
    }
    if (wholedec().isValid()) {
        return wholedec(move_back);
    }
    return ParseResult(-1, -1);
}

ParseResult MathParser::factor(bool move_back = true)
{
    if (whole().getVal() != -1) {
        return whole(move_back);
    }
    return ParseResult(-1, -1);
}

//
qlonglong applyOp(qlonglong a, qlonglong b, QChar op)
{
    if (op == '*') {

        return a * b;
    }
    else if (op == '/') {
        return a / b;
    }
    else if (op == '+') {
        return a + b;
    }
    else if (op == '-') {
        return a - b;
    }

    return 0;
}

int precedence(char op)
{
    if (op == '+' || op == '-') {
        return 1;
    }
    if (op == '*' || op == '/') {
        return 2;
    }
    return 0;
}

ParseResult MathParser::factors()
{
    QVector<qlonglong> nums;
    QVector<QChar> allOps;

    if (factor().isValid()) {
        nums.push_back(factor(false).getVal());

        while (!op().isNull()) {
            allOps.push_back(op());
            this->next();

            if (factor().isValid()) {
                nums.push_back(factor(false).getVal());
            }
            else {
                return ParseResult(-1, -1);
            }
        }
        // Calculation

        qlonglong result = nums[0];
        if (allOps.size() != 0) {
            for (int i = 0; i < allOps.size(); i++) {
                if (allOps[i] == "*" || allOps[i] == "/") {
                    result = applyOp(nums[i], nums[i + 1], allOps[i]);
                    nums.erase(nums.begin() + i);
                    nums.erase(nums.begin() + i);
                    nums.insert(nums.begin() + i, result);
                    allOps.erase(allOps.begin() + i);
                    i--;
                }
            }
            for (int i = 0; i < allOps.size(); i++) {
                if (allOps[i] == "+" || allOps[i] == "-") {
                    result = applyOp(nums[i], nums[i + 1], allOps[i]);
                    nums.erase(nums.begin() + i);
                    nums.erase(nums.begin() + i);
                    nums.insert(nums.begin() + i, result);
                    allOps.erase(allOps.begin() + i);
                    i--;
                }
            }
        }

        return ParseResult(result, 1);

    }
    return ParseResult(-1, -1);
}

int MathParser::unit()
{
    // Bits
    if (this->tok() == "B") {
        this->next();
        if (this->tok() == "i") {
            this->next();
            if (this->tok() == "t") {
                this->next();
                if (this->tok() == "s") {
                    return 1;
                }
                else {
                    this->prev();
                    this->prev();
                    this->prev();
                }
            }
            else {
                this->prev();
                this->prev();
            }
        }
        else {
            this->prev();
        }
    }

    // Bytes
    if (this->tok() == "B") {
        this->next();
        if (this->tok() == "y") {
            this->next();
            if (this->tok() == "t") {
                this->next();
                if (this->tok() == "e") {
                    this->next();
                    if (this->tok() == "s") {
                        return 8;
                    }
                    else {
                        this->prev();
                        this->prev();
                        this->prev();
                        this->prev();
                    }
                }
                else {
                    this->prev();
                    this->prev();
                    this->prev();
                }
            }
            else {
                this->prev();
                this->prev();
            }
        }
        else {
            this->prev();
        }
    }

    return -1;
}

ParseResult MathParser::base()
{
    qlonglong f = factors().getVal();
    if (f != -1) {
        if (this->tok() == ';') {
            return ParseResult(f, 1); // Bits is default unit
        }
        else {
            int u = this->unit();
            if (u != -1) {
                return ParseResult(f, u);
            }
            else {
                this->prev();
                this->prev();
                return ParseResult(-1, -1);
            }
        }
    }
    return ParseResult(-1, -1);
}

ParseResult MathParser::parseInput(QString tokens, ParseType t, int start)
{
    this->tokens = tokens.simplified().replace(" ", "");
    this->pos = start;
    ParseResult result;
    switch (t) {
    case Default:
        result = base();
        break;

    case No_Unit:
        result = factors();
        break;

    case Hex:
        result = wholehex(false);
        break;

    case Bin:
        result = wholebin(false);
        break;

    case Dec:
        result = wholedec(false);
        break;

    case Oct:
        result = wholeoct(false);
        break;

    case No_Math:
        result = whole(false);
        break;
    }
    return result;
}

ParseResult MathParser::parseInput(QString tokens)
{
    return this->parseInput(tokens, ParseType::Default);
}
