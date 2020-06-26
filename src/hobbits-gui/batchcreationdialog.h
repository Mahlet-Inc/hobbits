#ifndef BATCHCREATIONDIALOG_H
#define BATCHCREATIONDIALOG_H

#include <QDialog>
#include "bitcontainer.h"
#include "pluginactionbatch.h"

namespace Ui {
class BatchCreationDialog;
}

class BatchCreationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BatchCreationDialog(QSharedPointer<BitContainer> container, QWidget *parent = nullptr);
    ~BatchCreationDialog();

    int getSelectedBatchMode();

private slots:
    void adjustToCurrentSelections();

private:
    Ui::BatchCreationDialog *ui;
};

#endif // BATCHCREATIONDIALOG_H
