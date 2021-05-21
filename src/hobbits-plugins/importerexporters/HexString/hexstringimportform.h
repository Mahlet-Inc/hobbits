#ifndef HEXSTRINGIMPORTER_H
#define HEXSTRINGIMPORTER_H

#include "importresult.h"
#include "abstractparametereditor.h"

namespace Ui
{
class HexStringImportForm;
}

class HexStringImportForm : public AbstractParameterEditor
{
    Q_OBJECT

public:
    explicit HexStringImportForm();
    ~HexStringImportForm() override;

    QString title() override;

    bool setParameters(const Parameters &parameters) override;
    Parameters parameters() override;
    bool isStandaloneDialog() override;

    static QSharedPointer<ImportResult> importFromFile(QString fileName);
    static QSharedPointer<ImportResult> importFromHexString(QString hexString, int repeats);

private slots:
    void on_te_hexString_textChanged();
    void on_pb_selectFile_pressed();
    void on_pb_submitInput_pressed();
    void on_cb_repeat_toggled(bool checked);

private:
    Ui::HexStringImportForm *ui;
    QString m_fileName;
};

#endif // HEXSTRINGIMPORTER_H
