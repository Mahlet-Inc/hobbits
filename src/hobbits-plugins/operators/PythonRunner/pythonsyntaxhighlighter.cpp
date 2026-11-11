#include "pythonsyntaxhighlighter.h"

PythonSyntaxHighlighter::PythonSyntaxHighlighter(QTextDocument *parent) :
    QSyntaxHighlighter(parent)
{
    keywords = QStringList() << "and" << "assert" << "break" << "class" << "continue" << "def"
                             << "del" << "elif" << "else" << "except" << "exec" << "finally"
                             << "for" << "from" << "global" << "if" << "import" << "in"
                             << "is" << "lambda" << "not" << "or" << "pass" << "print"
                             << "raise" << "return" << "try" << "while" << "yield"
                             << "None" << "True" << "False";

    operators = QStringList() << "="
                              <<// Comparison
                "==" << "!=" << "<" << "<=" << ">" << ">="
                              <<// Arithmetic
                "\\+" << "-" << "\\*" << "/" << "//" << "%" << "\\*\\*"
                              <<// In-place
                "\\+=" << "-=" << "\\*=" << "/=" << "%="
                              <<// Bitwise
                "\\^" << "\\|" << "&" << "~" << ">>" << "<<";

    braces = QStringList() << "{" << "}" << "\\(" << "\\)" << "\\[" << "]";

    basicStyles.insert("keyword", getTextCharFormat("paleturquoise"));
    basicStyles.insert("operator", getTextCharFormat("peachpuff"));
    basicStyles.insert("defclass", getTextCharFormat("peachpuff", "bold"));
    basicStyles.insert("brace", getTextCharFormat("peachpuff"));
    basicStyles.insert("string", getTextCharFormat("thistle"));
    basicStyles.insert("string2", getTextCharFormat("plum"));
    basicStyles.insert("comment", getTextCharFormat("greenyellow", "italic"));
    basicStyles.insert("self", getTextCharFormat("tomato", "italic"));
    basicStyles.insert("numbers", getTextCharFormat("orange"));

    triSingleQuote.setPattern("'''");
    triDoubleQuote.setPattern("\"\"\"");

    initializeRules();
}

void PythonSyntaxHighlighter::initializeRules()
{
    for (QString currKeyword : keywords)
    {
        rules.append(HighlightingRule(QString("\\b%1\\b").arg(currKeyword), 0, basicStyles.value("keyword")));
    }
    for (QString currOperator : operators)
    {
        rules.append(HighlightingRule(QString("%1").arg(currOperator), 0, basicStyles.value("operator")));
    }
    for (QString currBrace : braces)
    {
        rules.append(HighlightingRule(QString("%1").arg(currBrace), 0, basicStyles.value("brace")));
    }
    // 'self'
    rules.append(HighlightingRule("\\bself\\b", 0, basicStyles.value("self")));

    // Double-quoted string, possibly containing escape sequences
    // FF: originally in python : r'"[^"\\]*(\\.[^"\\]*)*"'
    rules.append(HighlightingRule("\"[^\"\\\\]*(\\\\.[^\"\\\\]*)*\"", 0, basicStyles.value("string")));
    // Single-quoted string, possibly containing escape sequences
    // FF: originally in python : r"'[^'\\]*(\\.[^'\\]*)*'"
    rules.append(HighlightingRule("'[^'\\\\]*(\\\\.[^'\\\\]*)*'", 0, basicStyles.value("string")));

    // 'def' followed by an identifier
    // FF: originally: r'\bdef\b\s*(\w+)'
    rules.append(HighlightingRule("\\bdef\\b\\s*(\\w+)", 1, basicStyles.value("defclass")));
    // 'class' followed by an identifier
    // FF: originally: r'\bclass\b\s*(\w+)'
    rules.append(HighlightingRule("\\bclass\\b\\s*(\\w+)", 1, basicStyles.value("defclass")));

    // From '#' until a newline
    // FF: originally: r'#[^\\n]*'
    rules.append(HighlightingRule("#[^\\n]*", 0, basicStyles.value("comment")));

    // Numeric literals
    rules.append(HighlightingRule("\\b[+-]?[0-9]+[lL]?\\b", 0, basicStyles.value("numbers"))); // r'\b[+-]?[0-9]+[lL]?\b'
    rules.append(HighlightingRule("\\b[+-]?0[xX][0-9A-Fa-f]+[lL]?\\b", 0, basicStyles.value("numbers"))); // r'\b[+-]?0[xX][0-9A-Fa-f]+[lL]?\b'
    rules.append(
            HighlightingRule(
                    "\\b[+-]?[0-9]+(?:\\.[0-9]+)?(?:[eE][+-]?[0-9]+)?\\b",
                    0,
                    basicStyles.value(
                            "numbers")));                                                                                   // r'\b[+-]?[0-9]+(?:\.[0-9]+)?(?:[eE][+-]?[0-9]+)?\b'
}

void PythonSyntaxHighlighter::highlightBlock(const QString &text)
{
    for (const HighlightingRule &rule : rules) {
        auto matches = rule.pattern.globalMatch(text);
        while (matches.hasNext()) {
            const auto match = matches.next();
            setFormat(match.capturedStart(rule.nth), match.capturedLength(rule.nth), rule.format);
        }
    }

    setCurrentBlockState(0);

    // Do multi-line strings
    bool isInMultilne = matchMultiline(text, triSingleQuote, 1, basicStyles.value("string2"));
    if (!isInMultilne) {
        isInMultilne = matchMultiline(text, triDoubleQuote, 2, basicStyles.value("string2"));
    }
}

bool PythonSyntaxHighlighter::matchMultiline(
        const QString &text,
        const QRegularExpression &delimiter,
        const int inState,
        const QTextCharFormat &style)
{
    const auto first = delimiter.match(text);
    int start = previousBlockState() == inState ? 0 : first.capturedStart();
    int openingLength = previousBlockState() == inState ? 0 : first.capturedLength();

    while (start >= 0) {
        const auto closing = delimiter.match(text, start + openingLength);
        if (!closing.hasMatch()) {
            setFormat(start, text.size() - start, style);
            setCurrentBlockState(inState);
            return true;
        }
        const int end = closing.capturedEnd();
        setFormat(start, end - start, style);
        setCurrentBlockState(0);
        const auto next = delimiter.match(text, end);
        start = next.capturedStart();
        openingLength = next.capturedLength();
    }
    return false;
}

const QTextCharFormat PythonSyntaxHighlighter::getTextCharFormat(const QString &colorName, const QString &style)
{
    QTextCharFormat charFormat;
    QColor color(colorName);
    charFormat.setForeground(color);
    if (style.contains("bold", Qt::CaseInsensitive)) {
        charFormat.setFontWeight(QFont::Bold);
    }
    if (style.contains("italic", Qt::CaseInsensitive)) {
        charFormat.setFontItalic(true);
    }
    return charFormat;
}
