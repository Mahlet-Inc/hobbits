#include "hexstringimportform.h"
#include "settingsmanager.h"
#include "ui_hexstringimportform.h"
#include <QFileDialog>
#include <QMessageBox>

HexStringImportForm::HexStringImportForm() :
    ui(new Ui::HexStringImportForm)
{
    ui->setupUi(this);
}

HexStringImportForm::~HexStringImportForm()
{
    delete ui;
}

QString HexStringImportForm::title()
{
    return "Import Hex String";
}

bool HexStringImportForm::setParameters(const Parameters &parameters)
{
    if (parameters.isNull()) {
        return false;
    }

    if (parameters.contains("hex_string")) {
        QString hexString = parameters.value("hex_string").toString();
        ui->te_hexString->setPlainText(hexString);
    }
    if (parameters.contains("repeats")) {
        int repeats = parameters.value("repeats").toInt();
        if (repeats > 0) {
            ui->cb_repeat->setChecked(true);
            ui->sb_repeats->setValue(repeats);
        }
    }

    return true;
}

Parameters HexStringImportForm::parameters()
{
    Parameters params;
    if (!m_fileName.isEmpty()) {
        params.insert("filename", m_fileName);
    }
    else if (!ui->te_hexString->toPlainText().isEmpty()) {
        params.insert("hex_string", ui->te_hexString->toPlainText());
        if (ui->cb_repeat->isChecked()) {
            params.insert("repeats", ui->sb_repeats->value());
        }
        else {
            params.insert("repeats", 1);
        }
    }
    return params;
}

bool HexStringImportForm::isStandaloneDialog()
{
    return true;
}

void HexStringImportForm::on_te_hexString_textChanged()
{
    ui->pb_submitInput->setEnabled(!ui->te_hexString->toPlainText().isEmpty());
}

QSharedPointer<ImportResult> HexStringImportForm::importFromFile(QString fileName) {
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly)) {
        return ImportResult::error(QString("Failed to open hex string file: '%1'").arg(fileName));
    }

    QTemporaryFile bufferFile;
    if (!bufferFile.open()) {
        return ImportResult::error(QString("Failed to open temporary buffer file: '%1'").arg(bufferFile.fileName()));
    }

    while (file.bytesAvailable() > 0) {
        QByteArray hexEncoded = file.read(100000);
        QByteArray data = QByteArray::fromHex(hexEncoded);
        bufferFile.write(data);
    }

    bufferFile.seek(0);

    if (bufferFile.bytesAvailable() < 1) {
        return ImportResult::error(QString("Failed to read hex-encoded string data from: '%1'").arg(fileName));
    }

    auto container = BitContainer::create(&bufferFile);
    container->setName(QString("hex decode < %1").arg(QFileInfo(file).baseName()));

    Parameters params;
    params.insert("filename", fileName);
    return ImportResult::result(container, params);
}

QSharedPointer<ImportResult> HexStringImportForm::importFromHexString(QString hexString, int repeats)
{
    QByteArray data = QByteArray::fromHex(hexString.toLatin1());
    if (data.isEmpty()) {
        return ImportResult::error(QString("Failed to import hex-encoded string data"));
    }

    QTemporaryFile bufferFile;
    if (!bufferFile.open()) {
        return ImportResult::error(QString("Failed to open temporary buffer file: '%1'").arg(bufferFile.fileName()));
    }

    for (int i = 0; i < repeats; i++) {
        bufferFile.write(data);
    }

    bufferFile.seek(0);

    if (bufferFile.bytesAvailable() < 1) {
        return ImportResult::error(QString("Failed to process hex-encoded string data"));
    }

    auto container = BitContainer::create(&bufferFile);
    container->setName("hex input");

    Parameters params;
    params.insert("hex_string", hexString);
    params.insert("repeats", repeats);
    return ImportResult::result(container, params);
}

void HexStringImportForm::on_pb_selectFile_pressed()
{
    m_fileName = QFileDialog::getOpenFileName(
            this,
            tr("Import Hex String File"),
            SettingsManager::getPrivateSetting(SettingsManager::LAST_IMPORT_EXPORT_PATH_KEY).toString(),
            tr("All Files (*)"));

    if (!m_fileName.isEmpty()) {
        emit accepted();
    }
}

void HexStringImportForm::on_pb_submitInput_pressed()
{
    emit accepted();
}

void HexStringImportForm::on_cb_repeat_toggled(bool checked)
{
    ui->sb_repeats->setEnabled(checked);
}

