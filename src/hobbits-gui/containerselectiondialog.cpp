#include "containerselectiondialog.h"
#include "ui_containerselectiondialog.h"

ContainerSelectionDialog::ContainerSelectionDialog(QSharedPointer<BitContainerManagerUi> manager, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ContainerSelectionDialog),
    m_currSelectionModel(QSharedPointer<QItemSelectionModel>(new QItemSelectionModel)),
    m_bitManager(manager)
{
    ui->setupUi(this);

    m_currSelectionModel->setModel(manager->getTreeModel().data());
    ui->tv_containers->setModel(manager->getTreeModel().data());
    ui->tv_containers->setSelectionModel(m_currSelectionModel.data());

    setWindowTitle("Select Input Containers");
}

ContainerSelectionDialog::~ContainerSelectionDialog()
{
    delete ui;
}

QList<QSharedPointer<BitContainer>> ContainerSelectionDialog::getSelected()
{
    QList<QSharedPointer<BitContainer>> selected;

    for (auto idx : m_currSelectionModel->selectedIndexes()) {
        selected.append(m_bitManager->getTreeModel()->getContainer(idx));
    }

    return selected;
}

void ContainerSelectionDialog::setMessage(QString msg)
{
    ui->lb_msg->setText(msg);
}
