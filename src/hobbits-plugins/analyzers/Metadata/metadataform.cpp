#include "metadataform.h"
#include "ui_metadataform.h"

MetadataForm::MetadataForm(QSharedPointer<ParameterDelegate> delegate):
    ui(new Ui::MetadataForm()),
    m_paramHelper(new ParameterHelper(delegate))
{
    ui->setupUi(this);

    m_paramHelper->addLineEditStringParameter("label", ui->le_metadataLabel);
    m_paramHelper->addTextEditStringParameter("contents", ui->te_metadataContents);
}

MetadataForm::~MetadataForm()
{
    delete ui;
}

QString MetadataForm::title()
{
    return "Configure Metadata";
}

QJsonObject MetadataForm::parameters()
{
    return m_paramHelper->getParametersFromUi();
}

bool MetadataForm::setParameters(QJsonObject parameters)
{
    return m_paramHelper->applyParametersToUi(parameters);
}

void MetadataForm::previewBitsUiImpl(QSharedPointer<BitContainerPreview> container)
{
    while (ui->formLayout->rowCount() > 0) {
        ui->formLayout->removeRow(0);
    }

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
