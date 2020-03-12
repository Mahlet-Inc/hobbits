#include "asciiviewwidget.h"

#include <QImage>
#include <QPainter>

AsciiViewWidget::AsciiViewWidget(QSharedPointer<DisplayHandle> displayHandle, DisplayInterface *pluginRef) :
    DisplayBaseText(displayHandle, pluginRef, 1)
{

}

QString AsciiViewWidget::getDisplayChars(Frame frame, int offset)
{
    if (!m_currCharMaker.isNull()) {
        return m_currCharMaker->getDisplayChars(frame, offset);
    }
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


void AsciiViewWidget::setCharMakers(QList<QSharedPointer<CharMaker>> charMakers) {
    m_charMakers.clear();
    for (auto maker : charMakers) {
        m_charMakers.insert(maker->getName(), maker);
    }

    if (!charMakers.isEmpty()) {
        setCurrCharMaker(charMakers.first()->getName());
    }
}

void AsciiViewWidget::setCurrCharMaker(QString charMaker) {
    m_currCharMaker = m_charMakers.value(charMaker);
    repaint();
}
