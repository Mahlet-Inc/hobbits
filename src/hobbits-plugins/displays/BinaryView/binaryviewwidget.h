#ifndef BINARYVIEWWIDGET_H
#define BINARYVIEWWIDGET_H

#include "displaybasetext.h"

class BinaryViewWidget : public DisplayBaseText
{
    Q_OBJECT

public:
    BinaryViewWidget(QSharedPointer<DisplayHandle> displayHandle, DisplayInterface *pluginRef);

    QString getDisplayChars(Frame frame, int offset) override;
    int bitsPerChar() override;

};

#endif // BINARYVIEWWIDGET_H
