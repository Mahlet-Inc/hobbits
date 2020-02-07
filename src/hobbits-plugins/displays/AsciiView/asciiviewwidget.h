#ifndef ASCIIVIEWWIDGET_H
#define ASCIIVIEWWIDGET_H

#include "displaybasetext.h"

class AsciiViewWidget : public DisplayBaseText
{
    Q_OBJECT

public:
    AsciiViewWidget(QSharedPointer<DisplayHandle> displayHandle, DisplayInterface *pluginRef);

    QString getDisplayChars(Frame frame, int offset) override;
    int bitsPerChar() override;

};

#endif // ASCIIVIEWWIDGET_H
