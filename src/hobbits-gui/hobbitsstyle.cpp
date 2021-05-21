#include "hobbitsstyle.h"
#include <QFile>
#include <QFontDatabase>
#include <QStyleOption>
#include <QPainter>

HobbitsStyle::HobbitsStyle()
{

}

void HobbitsStyle::applyStyle(QApplication &app)
{
    loadFonts();

    QFont font("Roboto", 10);
    font.setStyleHint(QFont::SansSerif);
    QApplication::setFont(font);

    QFile styleFile(":/style/qss/style.qss");
    if (styleFile.open(QFile::ReadOnly)) {
        QString appStyle = styleFile.readAll();
        app.setStyleSheet(appStyle);
    }
}

void HobbitsStyle::loadFonts()
{
    QFontDatabase::addApplicationFont(":/style/fonts/Montserrat/Montserrat-Regular.ttf");
    QFontDatabase::addApplicationFont(":/style/fonts/Montserrat/Montserrat-Medium.ttf");
    QFontDatabase::addApplicationFont(":/style/fonts/Montserrat/Montserrat-Bold.ttf");

    QFontDatabase::addApplicationFont(":/style/fonts/Roboto/Roboto-Thin.ttf");
    QFontDatabase::addApplicationFont(":/style/fonts/Roboto/Roboto-ThinItalic.ttf");
    QFontDatabase::addApplicationFont(":/style/fonts/Roboto/Roboto-Light.ttf");
    QFontDatabase::addApplicationFont(":/style/fonts/Roboto/Roboto-LightItalic.ttf");
    QFontDatabase::addApplicationFont(":/style/fonts/Roboto/Roboto-Regular.ttf");
    QFontDatabase::addApplicationFont(":/style/fonts/Roboto/Roboto-Italic.ttf");
    QFontDatabase::addApplicationFont(":/style/fonts/Roboto/Roboto-Medium.ttf");
    QFontDatabase::addApplicationFont(":/style/fonts/Roboto/Roboto-MediumItalic.ttf");
    QFontDatabase::addApplicationFont(":/style/fonts/Roboto/Roboto-Bold.ttf");
    QFontDatabase::addApplicationFont(":/style/fonts/Roboto/Roboto-BoldItalic.ttf");
    QFontDatabase::addApplicationFont(":/style/fonts/Roboto/Roboto-Black.ttf");
    QFontDatabase::addApplicationFont(":/style/fonts/Roboto/Roboto-BlackItalic.ttf");

    QFontDatabase::addApplicationFont(":/style/fonts/Roboto_Mono/RobotoMono-VariableFont_wght.ttf");
    QFontDatabase::addApplicationFont(":/style/fonts/Roboto_Mono/static/RobotoMono-Regular.ttf");
    QFontDatabase::addApplicationFont(":/style/fonts/Roboto_Mono/static/RobotoMono-Medium.ttf");
    QFontDatabase::addApplicationFont(":/style/fonts/Roboto_Mono/static/RobotoMono-Bold.ttf");
}
