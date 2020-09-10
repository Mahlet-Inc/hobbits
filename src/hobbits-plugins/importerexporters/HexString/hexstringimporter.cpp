#include "hexstringimporter.h"
#include "settingsmanager.h"
#include "ui_hexstringimporter.h"
#include <QFileDialog>
#include <QMessageBox>

HexStringImporter::HexStringImporter(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HexStringImporter)
{
    ui->setupUi(this);

    setWindowTitle("Import Hex String");
}

HexStringImporter::~HexStringImporter()
{
    delete ui;
}

QSharedPointer<BitContainer> HexStringImporter::getContainer() const
{
    return m_container;
}


QString HexStringImporter::getFileName() const
{
    return m_fileName;
}

QString HexStringImporter::getHexString() const
{
    return ui->te_hexString->toPlainText();
}

int HexStringImporter::getRepeats() const
{
    if (ui->cb_repeat->isChecked()) {
        return ui->sb_repeats->value();
    }
    else {
        return 1;
    }
}

void HexStringImporter::on_te_hexString_textChanged()
{
    ui->pb_submitInput->setEnabled(!ui->te_hexString->toPlainText().isEmpty());
}

void HexStringImporter::importFromFile(QString fileName) {
    m_fileName = fileName;
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox msg(this);
        msg.setWindowTitle("Import Bits Error");
        msg.setText(QString("Failed to import bit file: '%1'").arg(fileName));
        msg.setDefaultButton(QMessageBox::Ok);
        msg.exec();
        return;
    }

    QByteArray hexEncoded = file.readAll();
    QByteArray data = QByteArray::fromHex(hexEncoded);

    if (data.isEmpty()) {
        QMessageBox msg(this);
        msg.setWindowTitle("Import Bits Error");
        msg.setText(QString("Failed to import hex-encoded string data from: '%1'").arg(fileName));
        msg.setDefaultButton(QMessageBox::Ok);
        msg.exec();
        return;
    }

    m_container = BitContainer::create(data);
    m_container->setName(QString("hex decode < %1").arg(QFileInfo(file).baseName()));
}

void HexStringImporter::importFromHexString(QString hexString, int repeats)
{
    ui->te_hexString->setPlainText(hexString);
    if (repeats > 1) {
        ui->cb_repeat->setChecked(true);
        ui->sb_repeats->setValue(repeats);
    }
    else {
        ui->cb_repeat->setChecked(false);
    }

    QByteArray data = QByteArray::fromHex(hexString.toLatin1());

    if (data.isEmpty()) {
        QMessageBox msg(this);
        msg.setWindowTitle("Import Bits Error");
        msg.setText(QString("Failed to import hex-encoded string data"));
        msg.setDefaultButton(QMessageBox::Ok);
        msg.exec();
        return;
    }

    auto baseData = data;
    for (int i = 1; i < repeats; i++) {
        data.append(baseData);
    }

    m_container = BitContainer::create(data);
    m_container->setName("hex input");

    this->accept();
}

void HexStringImporter::on_pb_selectFile_pressed()
{
    m_container = QSharedPointer<BitContainer>();
    QString fileName = QFileDialog::getOpenFileName(
            this,
            tr("Import Hex String File"),
            SettingsManager::getInstance().getPrivateSetting(SettingsData::LAST_IMPORT_EXPORT_PATH_KEY).toString(),
            tr("All Files (*)"));

    if (fileName.isEmpty()) {
        return;
    }

    importFromFile(fileName);

    this->accept();
}

void HexStringImporter::on_pb_submitInput_pressed()
{
    int repeats = 1;
    if (ui->cb_repeat->isChecked()) {
        repeats = ui->sb_repeats->value();
    }

    importFromHexString(ui->te_hexString->toPlainText(), repeats);
}

void HexStringImporter::on_cb_repeat_toggled(bool checked)
{
    ui->sb_repeats->setEnabled(checked);
}
