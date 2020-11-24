#ifndef HOBBITSSTYLE_H
#define HOBBITSSTYLE_H

#include <QApplication>

class HobbitsStyle
{
public:
    HobbitsStyle();

    static void applyStyle(QApplication &app);

private:
    static void loadFonts();
};

#endif // HOBBITSSTYLE_H
