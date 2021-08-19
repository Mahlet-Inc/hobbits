#include "headerframerform.h"
#include "ui_headerframerform.h"
#include <QJsonArray>

HeaderFramerForm::HeaderFramerForm(QSharedPointer<ParameterDelegate> delegate) :
    ui(new Ui::HeaderFramerForm()),
    m_delegate(delegate)
{
    ui->setupUi(this);

    connect(ui->le_header,& QLineEdit::textChanged, this, &HeaderFramerForm::validateHeader);

    connect(ui->pb_add, SIGNAL(pressed()), this, SLOT(addHeader()));
    connect(ui->pb_remove, SIGNAL(pressed()), this, SLOT(removeHeader()));

    connect(ui->tw_headers, SIGNAL(itemSelectionChanged()), this, SLOT(checkSelectedHeader()));

    ui->pb_add->setEnabled(false);
    ui->pb_remove->setEnabled(false);
    ui->tw_headers->setColumnCount(4);
    ui->tw_headers->setHorizontalHeaderLabels({"Frame Header", "Frame Length", "Pre-pad", "Byte-aligned"});
    ui->sb_frameLength->setMaximum(INT32_MAX);

    ui->sb_prePad->setMinimum(0);
    ui->sb_prePad->setMaximum(INT32_MAX);

    connect(ui->cb_prePad, SIGNAL(stateChanged(int)), this, SLOT(showSpinBoxes()));
    connect(ui->cb_fixedLength, SIGNAL(stateChanged(int)), this, SLOT(showSpinBoxes()));
    showSpinBoxes();
}

HeaderFramerForm::~HeaderFramerForm()
{
    delete ui;
}

QString HeaderFramerForm::title()
{
    return "Configure Frame Headers";
}

bool HeaderFramerForm::setParameters(const Parameters &parameters)
{
    if (!m_delegate->validate(parameters).isEmpty()) {
        return false;
    }

    int row = 0;
    ui->tw_headers->clearContents();
    ui->tw_headers->setRowCount(0);
    for (QJsonValueRef valueRef: parameters.value("headers").toArray()) {
        QJsonObject headerData = valueRef.toObject();

        ui->tw_headers->insertRow(ui->tw_headers->rowCount());
        ui->tw_headers->setItem(row, 0, new QTableWidgetItem(headerData.value("header").toString()));
        ui->tw_headers->setItem(row, 1, new QTableWidgetItem(headerData.value("length").toString()));

        if (headerData.contains("pre-pad") && headerData.value("pre-pad").isDouble()) {
            auto item = new QTableWidgetItem(QString("%1").arg(headerData.value("pre-pad").toDouble()));
            item->setData(Qt::UserRole, headerData.value("pre-pad"));
            ui->tw_headers->setItem(row, 2, item);
        }
        if (headerData.contains("byte-aligned") && headerData.value("byte-aligned").isBool()) {
            auto item = new QTableWidgetItem(headerData.value("byte-aligned").toBool() ? "true" : "false");
            item->setData(Qt::UserRole, headerData.value("byte-aligned"));
            ui->tw_headers->setItem(row, 3, item);
        }

        row++;
    }

    return true;
}

Parameters HeaderFramerForm::parameters()
{
    Parameters parameters;

    QJsonArray headersArray;
    for (int i = 0; i < ui->tw_headers->rowCount(); i++) {
        QJsonObject headerData;
        headerData.insert("header", ui->tw_headers->item(i, 0)->text());
        headerData.insert("length", ui->tw_headers->item(i, 1)->text());
        headerData.insert("pre-pad", ui->tw_headers->item(i, 2)->data(Qt::UserRole).toInt());
        headerData.insert("byte-aligned", ui->tw_headers->item(i, 3)->data(Qt::UserRole).toBool());
        headersArray.append(headerData);
    }

    parameters.insert("headers", headersArray);

    return parameters;
}

QString HeaderFramerForm::getHeaderString()
{
    // Parse header
    if (ui->le_header->text().isEmpty()) {
        return QString();
    }

    QStringList parseErrors;
    QSharedPointer<BitArray> header = BitArray::fromString(ui->le_header->text(), &parseErrors);
    if (!parseErrors.isEmpty()) {
        return QString();
    }

    ui->sb_frameLength->setMinimum(int(header->sizeInBits()));

    return ui->le_header->text();
}

void HeaderFramerForm::showSpinBoxes()
{
    if (ui->cb_prePad->isChecked()) {
        ui->sb_prePad->setVisible(true);
        ui->cb_prePad->setText("Pre-pad:");
    }
    else {
        ui->sb_prePad->setVisible(false);
        ui->cb_prePad->setText("Pre-pad");
    }
    if (ui->cb_fixedLength->isChecked()) {
        ui->sb_frameLength->setVisible(true);
        ui->cb_fixedLength->setText("Fixed Frame Length:");
    }
    else {
        ui->sb_frameLength->setVisible(false);
        ui->cb_fixedLength->setText("Fixed Frame Length");
    }
}

void HeaderFramerForm::validateHeader(QString header)
{
    Q_UNUSED(header)
    QString headerString = getHeaderString();
    ui->pb_add->setEnabled(!headerString.isEmpty());
}

void HeaderFramerForm::addHeader()
{
    QString headerString = getHeaderString();
    if (headerString.isEmpty()) {
        return;
    }

    int row = ui->tw_headers->rowCount();
    ui->tw_headers->insertRow(row);
    ui->tw_headers->setItem(row, 0, new QTableWidgetItem(headerString));
    if (ui->cb_fixedLength->isChecked()) {
        ui->tw_headers->setItem(row, 1, new QTableWidgetItem(QString("%1").arg(ui->sb_frameLength->value())));
    }
    else {
        ui->tw_headers->setItem(row, 1, new QTableWidgetItem("*"));
    }

    if (ui->cb_prePad->isChecked()) {
        auto item = new QTableWidgetItem(ui->sb_prePad->text());
        item->setData(Qt::UserRole, ui->sb_prePad->value());
        ui->tw_headers->setItem(row, 2, item);
    }
    else {
        auto item = new QTableWidgetItem("0");
        item->setData(Qt::UserRole, 0);
        ui->tw_headers->setItem(row, 2, item);
    }

    if (ui->cb_byteAlign->isChecked()) {
        auto item = new QTableWidgetItem("true");
        item->setData(Qt::UserRole, true);
        ui->tw_headers->setItem(row, 3, item);
    }
    else {
        auto item = new QTableWidgetItem("false");
        item->setData(Qt::UserRole, false);
        ui->tw_headers->setItem(row, 3, item);
    }
}

void HeaderFramerForm::checkSelectedHeader()
{
    ui->pb_remove->setEnabled(!ui->tw_headers->selectedItems().isEmpty());
}

void HeaderFramerForm::removeHeader()
{
    QSet<int> rows;
    for (auto item : ui->tw_headers->selectedItems()) {
        rows.insert(item->row());
    }
    QList<int> rowList = rows.values();
    std::sort(rowList.begin(), rowList.end());
    for (int i = rowList.length() - 1; i >= 0; i--) {
        ui->tw_headers->removeRow(rowList.at(i));
    }
}
