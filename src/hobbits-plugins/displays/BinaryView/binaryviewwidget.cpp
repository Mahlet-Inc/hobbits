#include "binaryviewwidget.h"

#include <QImage>
#include <QPainter>

BinaryViewWidget::BinaryViewWidget(QSharedPointer<DisplayHandle> displayHandle, DisplayInterface *pluginRef) :
    DisplayBaseText(displayHandle, pluginRef, 4)
{

}

QString BinaryViewWidget::getDisplayChars(Frame frame, int offset)
{
    if (frame.at(offset)) {
        return "1";
    }
    else {
        return "0";
    }
}

int BinaryViewWidget::bitsPerChar()
{
    return 1;
}
