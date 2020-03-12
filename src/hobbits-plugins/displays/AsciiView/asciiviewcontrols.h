#ifndef ASCIIVIEWCONTROLS_H
#define ASCIIVIEWCONTROLS_H

#include <QWidget>
#include "charmaker.h"

namespace Ui
{
class AsciiViewControls;
}

class AsciiViewControls : public QWidget
{
    Q_OBJECT

public:
    AsciiViewControls();

    void setCharMakers(QList<QSharedPointer<CharMaker>> charMakers);

signals:
    void fontSizeChanged(int);
    void columnGroupingChanged(int);
    void showHeadersChanged(bool);
    void charMakerChanged(QString);

private slots:
    void on_cb_showHeaders_stateChanged(int state);

    void on_cb_charMaker_currentTextChanged(const QString &charMakerName);

private:
    Ui::AsciiViewControls *ui;
};

#endif // ASCIIVIEWCONTROLS_H
