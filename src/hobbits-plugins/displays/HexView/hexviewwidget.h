#ifndef HEXVIEWWIDGET_H
#define HEXVIEWWIDGET_H

#include "displaybasetext.h"

class HexViewWidget : public DisplayBaseText
{
    Q_OBJECT

public:
    HexViewWidget(QSharedPointer<DisplayHandle> displayHandle, DisplayInterface *pluginRef);

    QString getDisplayChars(Frame frame, int offset) override;
    int bitsPerChar() override;

};

#endif // HEXVIEWWIDGET_H
