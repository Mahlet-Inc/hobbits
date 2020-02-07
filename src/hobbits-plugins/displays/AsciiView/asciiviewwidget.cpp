#include "asciiviewwidget.h"

#include <QImage>
#include <QPainter>

AsciiViewWidget::AsciiViewWidget(QSharedPointer<DisplayHandle> displayHandle, DisplayInterface *pluginRef) :
    DisplayBaseText(displayHandle, pluginRef, 1)
{

}

QString AsciiViewWidget::getDisplayChars(Frame frame, int offset)
{
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

int AsciiViewWidget::bitsPerChar()
{
    return 8;
}
