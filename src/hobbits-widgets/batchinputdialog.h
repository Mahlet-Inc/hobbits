#ifndef BATCHINPUTDIALOG_H
#define BATCHINPUTDIALOG_H

#include <QDialog>
#include <QMap>
#include <QSpinBox>
#include <QLabel>
#include <QStandardItemModel>

namespace Ui {
class BatchInputDialog;
}

class BatchInputDialog : public QDialog
{
    Q_OBJECT

public:
    explicit BatchInputDialog(QList<QPair<QUuid, int>> inputs, QMap<QUuid, QString> displayMap, QWidget *parent = nullptr);
    ~BatchInputDialog();

    QList<QPair<QUuid, int> > inputs() const;

private slots:
    void on_pb_delete_clicked();

private:
    Ui::BatchInputDialog *ui;
    QStandardItemModel *m_model;
};

#endif // BATCHINPUTDIALOG_H
