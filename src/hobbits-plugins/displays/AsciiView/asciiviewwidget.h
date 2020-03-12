#ifndef ASCIIVIEWWIDGET_H
#define ASCIIVIEWWIDGET_H

#include "displaybasetext.h"
#include "charmaker.h"

class AsciiViewWidget : public DisplayBaseText
{
    Q_OBJECT

public:
    AsciiViewWidget(QSharedPointer<DisplayHandle> displayHandle, DisplayInterface *pluginRef);

    QString getDisplayChars(Frame frame, int offset) override;
    int bitsPerChar() override;
    void setCharMakers(QList<QSharedPointer<CharMaker>>);

public slots:
    void setCurrCharMaker(QString charMaker);

private:
    QSharedPointer<CharMaker> m_currCharMaker;
    QMap<QString, QSharedPointer<CharMaker>> m_charMakers;

};

#endif // ASCIIVIEWWIDGET_H
