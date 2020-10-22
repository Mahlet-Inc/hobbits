#include "batchinputdialog.h"
#include "ui_batchinputdialog.h"
#include <QUuid>
#include <QStandardItemModel>
#include <QItemDelegate>
#include <QModelIndex>

BatchInputDialog::BatchInputDialog(QList<QPair<QUuid, int> > inputs, QMap<QUuid, QString> displayMap, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BatchInputDialog)
{
    ui->setupUi(this);

    m_model = new QStandardItemModel();

    for (auto input: inputs) {
        QStandardItem* name = new QStandardItem(displayMap.value(input.first));
        QStandardItem* outputValue = new QStandardItem();
        outputValue->setData(QVariant(input.second), Qt::EditRole);
        outputValue->setData(QVariant(input.first), Qt::UserRole);
        outputValue->setEditable(true);
        m_model->appendRow({name, outputValue});
    }
    m_model->setHorizontalHeaderLabels({"Source", "Output Number"});

    ui->tableView->setModel(m_model);
    ui->tableView->setItemDelegateForColumn(1, new QItemDelegate());
    ui->tableView->setColumnWidth(0, 250);
    ui->tableView->setColumnWidth(1, 100);

    ui->pb_delete->setEnabled(false);
    connect(ui->tableView->selectionModel(), &QItemSelectionModel::selectionChanged, [this]() {
        ui->pb_delete->setEnabled(ui->tableView->currentIndex().isValid());
    });

    setWindowTitle("Configure Step Sources");
}

BatchInputDialog::~BatchInputDialog()
{
    delete ui;
}

QList<QPair<QUuid, int> > BatchInputDialog::inputs() const
{
    QList<QPair<QUuid, int> > inputs;
    for (int i = 0; i < m_model->rowCount(); i++) {
        inputs.append({m_model->data(m_model->index(i, 1), Qt::UserRole).toUuid(), m_model->data(m_model->index(i, 1), Qt::EditRole).toInt()});
    }
    return inputs;
}

void BatchInputDialog::on_pb_delete_clicked()
{
    if (!ui->tableView->currentIndex().isValid()) {
        return;
    }
    m_model->removeRow(ui->tableView->currentIndex().row());
}
