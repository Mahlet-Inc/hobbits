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

QSharedPointer<ImportResult> HexStringImporter::getResult() const
{
    return m_result;
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

QSharedPointer<ImportResult> HexStringImporter::importFromFile(QString fileName) {
    m_result = ImportResult::nullResult();
    m_fileName = fileName;
    QFile file(fileName);

    if (!file.open(QIODevice::ReadOnly)) {
        m_result = ImportResult::error(QString("Failed to open hex string file: '%1'").arg(fileName));
        return m_result;
    }

    QTemporaryFile bufferFile;
    if (!bufferFile.open()) {
        m_result = ImportResult::error(QString("Failed to open temporary buffer file: '%1'").arg(bufferFile.fileName()));
        return m_result;
    }

    while (file.bytesAvailable() > 0) {
        QByteArray hexEncoded = file.read(100000);
        QByteArray data = QByteArray::fromHex(hexEncoded);
        bufferFile.write(data);
    }

    bufferFile.seek(0);

    if (bufferFile.bytesAvailable() < 1) {
        m_result = ImportResult::error(QString("Failed to read hex-encoded string data from: '%1'").arg(fileName));
        return m_result;
    }

    auto container = BitContainer::create(&bufferFile);
    container->setName(QString("hex decode < %1").arg(QFileInfo(file).baseName()));
    QJsonObject pluginState;
    pluginState.insert("filename", fileName);
    m_result = ImportResult::result(container, pluginState);

    return m_result;
}

QSharedPointer<ImportResult> HexStringImporter::importFromHexString(QString hexString, int repeats)
{
    m_result = ImportResult::nullResult();
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
        m_result = ImportResult::error(QString("Failed to import hex-encoded string data"));
        return m_result;
    }

    QTemporaryFile bufferFile;
    if (!bufferFile.open()) {
        m_result = ImportResult::error(QString("Failed to open temporary buffer file: '%1'").arg(bufferFile.fileName()));
        return m_result;
    }

    for (int i = 0; i < repeats; i++) {
        bufferFile.write(data);
    }

    bufferFile.seek(0);

    if (bufferFile.bytesAvailable() < 1) {
        m_result = ImportResult::error(QString("Failed to process hex-encoded string data"));
        return m_result;
    }

    auto container = BitContainer::create(&bufferFile);
    container->setName("hex input");
    QJsonObject pluginState;
    pluginState.insert("hex_string", hexString);
    pluginState.insert("repeats", repeats);
    m_result = ImportResult::result(container, pluginState);

    return m_result;
}

void HexStringImporter::on_pb_selectFile_pressed()
{
    QString fileName = QFileDialog::getOpenFileName(
            this,
            tr("Import Hex String File"),
            SettingsManager::getInstance().getPrivateSetting(SettingsData::LAST_IMPORT_EXPORT_PATH_KEY).toString(),
            tr("All Files (*)"));

    if (fileName.isEmpty()) {
        return;
    }

    importFromFile(fileName);

    if (!m_result.isNull() && m_result->errorString().isEmpty()) {
        this->accept();
    }
    else if (!m_result.isNull()) {
        QMessageBox msg(this);
        msg.setWindowTitle("Import Bits Error");
        msg.setText(m_result->errorString());
        msg.setDefaultButton(QMessageBox::Ok);
        msg.exec();
    }
}

void HexStringImporter::on_pb_submitInput_pressed()
{
    int repeats = 1;
    if (ui->cb_repeat->isChecked()) {
        repeats = ui->sb_repeats->value();
    }

    importFromHexString(ui->te_hexString->toPlainText(), repeats);

    if (!m_result.isNull() && m_result->errorString().isEmpty()) {
        this->accept();
    }
    else if (!m_result.isNull()) {
        QMessageBox msg(this);
        msg.setWindowTitle("Import Bits Error");
        msg.setText(m_result->errorString());
        msg.setDefaultButton(QMessageBox::Ok);
        msg.exec();
    }
}

void HexStringImporter::on_cb_repeat_toggled(bool checked)
{
    ui->sb_repeats->setEnabled(checked);
}

