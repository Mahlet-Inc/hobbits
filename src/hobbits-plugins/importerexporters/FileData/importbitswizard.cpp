#include "importbitswizard.h"
#include "ui_importbitswizard.h"

#include "settingsmanager.h"

#include <QFileDialog>
#include <QFileInfo>

ImportBitsWizard::ImportBitsWizard(QString fileName, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ImportBitsWizard),
    m_fileName(fileName)
{
    ui->setupUi(this);

    QFile file(fileName);
    file.open(QFile::ReadOnly);
    QByteArray previewData = file.read(100 * 256);
    file.close();

    BitContainer previewContainer;
    previewContainer.setBytes(previewData);

    ui->lb_imagePreview->setPixmap(previewContainer.getThumbnail());

    QFileInfo fileInfo(file);
    m_fileSizeKb = double(fileInfo.size()) / 1000.0;

    ui->lb_fileName->setText(QString("File: %1").arg(fileInfo.fileName()));
    ui->lb_fileSize->setText(QString("File Size: %1 kB").arg(m_fileSizeKb));

    ui->sb_startKb->setValue(0);
    ui->sb_startKb->setMaximum(m_fileSizeKb);

    adjustMaxBits();
    ui->sb_takeKb->setValue(ui->sb_takeKb->maximum());

    this->setWindowTitle("Import Bits");

    connect(ui->sb_startKb, SIGNAL(valueChanged(double)), this, SLOT(adjustMaxBits()));

    ui->le_templateName->setVisible(false);
}

ImportBitsWizard::~ImportBitsWizard()
{
    delete ui;
}

void ImportBitsWizard::adjustMaxBits()
{
    double maxKb = m_fileSizeKb - ui->sb_startKb->value();
    ui->sb_takeKb->setMinimum(0.001);
    ui->sb_takeKb->setMaximum(maxKb);
}

QSharedPointer<BitContainer> ImportBitsWizard::getImportedBitContainer()
{
    QFile file(m_fileName);

    qint64 byteOffset = qint64(ui->sb_startKb->value() * 1000);
    qint64 bytesToTake = qint64(ui->sb_takeKb->value() * 1000);

    file.open(QFile::ReadOnly);
    file.seek(byteOffset);
    QByteArray data = file.read(bytesToTake);
    QSharedPointer<BitContainer> container = QSharedPointer<BitContainer>(new BitContainer());
    container->setBytes(data, data.size() * 8);
    container->setName(QFileInfo(file).baseName());

    return container;
}
