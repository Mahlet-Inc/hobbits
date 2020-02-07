#include "hexviewwidget.h"

#include <QImage>
#include <QPainter>

HexViewWidget::HexViewWidget(QSharedPointer<DisplayHandle> displayHandle, DisplayInterface *pluginRef) :
    DisplayBaseText(displayHandle, pluginRef, 2)
{

}

QString HexViewWidget::getDisplayChars(Frame frame, int offset)
{
    QString frameString = "";
    if (offset + 3 >= frame.size()) {
        // partial nibble
        for (int bit = 0; bit < 4 && bit + offset < frame.size(); bit++) {
            if (frame.at(offset + bit)) {
                frameString += '.';
            }
            else {
                frameString += 'o';
            }
        }
    }
    else {
        int nibble = 0;
        for (int bit = 0; bit < 4; bit++) {
            nibble <<= 1;
            if (frame.at(offset + bit)) {
                nibble |= 0x01;
            }
        }
        QString nibString;
        nibString.setNum(nibble, 16);
        frameString += nibString;
    }
    return frameString;
}

int HexViewWidget::bitsPerChar()
{
    return 4;
}
