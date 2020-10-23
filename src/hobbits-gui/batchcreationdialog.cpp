#include "batchcreationdialog.h"
#include "ui_batchcreationdialog.h"

BatchCreationDialog::BatchCreationDialog(QSharedPointer<BitContainer> container, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::BatchCreationDialog)
{
    ui->setupUi(this);

    this->setWindowTitle("Batch Configuration");

    if (container->childUuids().isEmpty()) {
        ui->rb_after->setEnabled(false);
        ui->rb_before->setChecked(true);
    }
    else {
        ui->rb_after->setChecked(true);
    }

    connect(ui->rb_after, SIGNAL(toggled(bool)), this, SLOT(adjustToCurrentSelections()));
    connect(ui->rb_before, SIGNAL(toggled(bool)), this, SLOT(adjustToCurrentSelections()));
}

BatchCreationDialog::~BatchCreationDialog()
{
    delete ui;
}

int BatchCreationDialog::getSelectedBatchMode()
{
    int mode;
    if (ui->rb_after->isChecked()) {
        mode = PluginActionBatch::InclusiveAfter;
    }
    else {
        mode = PluginActionBatch::InclusiveBefore | PluginActionBatch::IncludeImportersFull;
    }

    return mode;
}
