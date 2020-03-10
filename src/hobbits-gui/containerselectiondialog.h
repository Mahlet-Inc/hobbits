#ifndef CONTAINERSELECTIONDIALOG_H
#define CONTAINERSELECTIONDIALOG_H

#include <QDialog>
#include "bitcontainermanager.h"

namespace Ui {
class ContainerSelectionDialog;
}

class ContainerSelectionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ContainerSelectionDialog(QSharedPointer<BitContainerManager> manager, QWidget *parent = nullptr);
    ~ContainerSelectionDialog();

    QList<QSharedPointer<BitContainer>> getSelected();
    void setMessage(QString msg);

private:
    Ui::ContainerSelectionDialog *ui;
    QSharedPointer<QItemSelectionModel> m_currSelectionModel;
    QSharedPointer<BitContainerManager> m_bitManager;
};

#endif // CONTAINERSELECTIONDIALOG_H
