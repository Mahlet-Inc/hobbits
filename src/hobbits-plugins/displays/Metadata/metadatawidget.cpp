#include "metadatawidget.h"

#include <QPainter>
#include <QImage>
#include "displayhelper.h"
#include "ui_metadatawidget.h"
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QScrollBar>

MetadataWidget::MetadataWidget(
        QSharedPointer<DisplayHandle> displayHandle,
        DisplayInterface *pluginRef,
        QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MetadataWidget()),
    m_displayHandle(displayHandle),
    m_pluginRef(pluginRef)
{
    ui->setupUi(this);

    connect(m_displayHandle.data(), SIGNAL(containerChanged()), this, SLOT(adjustData()));
    connect(m_displayHandle.data(),
            SIGNAL(newFocusDisplays(QSet<DisplayInterface*>)),
            this,
            SLOT(checkFocus(QSet<DisplayInterface*>)));
    connect(m_displayHandle.data(), SIGNAL(newBitContainer()), this, SLOT(adjustData()));
}

void MetadataWidget::checkFocus(QSet<DisplayInterface *> displays)
{
    if (displays.contains(m_pluginRef)) {
        this->adjustData();
    }
}

void MetadataWidget::adjustData()
{
    if (!m_displayHandle->getCurrentFocusDisplays().contains(m_pluginRef)) {
        return;
    }

    while (ui->formLayout->rowCount() > 0) {
        ui->formLayout->removeRow(0);
    }

    if (m_displayHandle->getCurrentFocusDisplays().size() == 1) {
        m_displayHandle->getHScroll()->setVisible(false);
        m_displayHandle->getVScroll()->setVisible(false);
    }

    auto container = m_displayHandle->getContainer();
    if (container.isNull()) {
        return;
    }

    for (auto key: container->info()->metadataKeys()) {
        QVariant metadata = container->info()->metadata(key);
        QString s = metadata.toString();
        if (s.size() < 100) {
            QLineEdit *lineEdit = new QLineEdit();
            lineEdit->setText(s);
            lineEdit->setReadOnly(true);
            ui->formLayout->addRow(key, lineEdit);        }
        else {
            auto textEdit = new QPlainTextEdit();
            textEdit->setPlainText(s);
            textEdit->setReadOnly(true);
            ui->formLayout->addRow(key, textEdit);
        }
    }
}

