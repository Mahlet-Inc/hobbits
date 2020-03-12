#include "asciiviewcontrols.h"
#include "ui_asciiviewcontrols.h"


AsciiViewControls::AsciiViewControls() :
    ui(new Ui::AsciiViewControls())
{
    ui->setupUi(this);

    connect(ui->sb_columnGrouping, SIGNAL(valueChanged(int)), this, SIGNAL(columnGroupingChanged(int)));
    connect(ui->hs_fontSize, SIGNAL(valueChanged(int)), this, SIGNAL(fontSizeChanged(int)));
}


void AsciiViewControls::setCharMakers(QList<QSharedPointer<CharMaker>> charMakers) {
    ui->cb_charMaker->clear();
    for (auto maker : charMakers) {
        ui->cb_charMaker->addItem(maker->getName());
    }
}

void AsciiViewControls::on_cb_showHeaders_stateChanged(int state)
{
    emit showHeadersChanged(state != Qt::Unchecked);
}

void AsciiViewControls::on_cb_charMaker_currentTextChanged(const QString &charMakerName)
{
    emit charMakerChanged(charMakerName);
}
