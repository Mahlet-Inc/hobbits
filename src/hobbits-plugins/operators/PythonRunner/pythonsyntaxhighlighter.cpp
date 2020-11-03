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
    for (HighlightingRule currRule : rules)
    {
        int idx = currRule.pattern.indexIn(text, 0);
        while (idx >= 0) {
            // Get index of Nth match
            idx = currRule.pattern.pos(currRule.nth);
            int length = currRule.pattern.cap(currRule.nth).length();
            setFormat(idx, length, currRule.format);
            idx = currRule.pattern.indexIn(text, idx + length);
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
        const QRegExp &delimiter,
        const int inState,
        const QTextCharFormat &style)
{
    int start = -1;
    int add = -1;
    int end = -1;
    int length = 0;

    // If inside triple-single quotes, start at 0
    if (previousBlockState() == inState) {
        start = 0;
        add = 0;
    }
    // Otherwise, look for the delimiter on this line
    else {
        start = delimiter.indexIn(text);
        // Move past this match
        add = delimiter.matchedLength();
    }

    // As long as there's a delimiter match on this line...
    while (start >= 0) {
        // Look for the ending delimiter
        end = delimiter.indexIn(text, start + add);
        // Ending delimiter on this line?
        if (end >= add) {
            length = end - start + add + delimiter.matchedLength();
            setCurrentBlockState(0);
        }
        // No; multi-line string
        else {
            setCurrentBlockState(inState);
            length = text.length() - start + add;
        }
        // Apply formatting and look for next
        setFormat(start, length, style);
        start = delimiter.indexIn(text, start + length);
    }
    // Return True if still inside a multi-line string, False otherwise
    if (currentBlockState() == inState) {
        return true;
    }
    else {
        return false;
    }
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
