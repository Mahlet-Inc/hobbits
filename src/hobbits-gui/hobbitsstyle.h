#ifndef HOBBITSSTYLE_H
#define HOBBITSSTYLE_H

#include <QApplication>
#include <QProxyStyle>

class HobbitsStyle : public QProxyStyle
{
public:
    HobbitsStyle();

    void drawPrimitive(QStyle::PrimitiveElement element, const QStyleOption *option, QPainter *painter, const QWidget *widget = nullptr) const override;

    static void applyStyle(QApplication &app);

private:
    static void loadFonts();
};

#endif // HOBBITSSTYLE_H
