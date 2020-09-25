#ifndef HEXSTRINGIMPORTER_H
#define HEXSTRINGIMPORTER_H

#include "importresult.h"
#include <QDialog>

namespace Ui
{
class HexStringImporter;
}

class HexStringImporter : public QDialog
{
    Q_OBJECT

public:
    explicit HexStringImporter(QWidget *parent = nullptr);

    ~HexStringImporter();

    QSharedPointer<ImportResult> importFromFile(QString fileName);
    QSharedPointer<ImportResult> importFromHexString(QString hexString, int repeats);
    QSharedPointer<ImportResult> getResult() const;
    QString getFileName() const;
    QString getHexString() const;
    int getRepeats() const;


private slots:
    void on_te_hexString_textChanged();
    void on_pb_selectFile_pressed();
    void on_pb_submitInput_pressed();

    void on_cb_repeat_toggled(bool checked);

private:
    Ui::HexStringImporter *ui;

    QSharedPointer<ImportResult> m_result;
    QString m_fileName;
};

#endif // HEXSTRINGIMPORTER_H
