#ifndef IMPORTBITSWIZARD_H
#define IMPORTBITSWIZARD_H

#include <QDialog>
#include <QFile>

#include "bitcontainer.h"

namespace Ui
{
class ImportBitsWizard;
}

class ImportBitsWizard : public QDialog
{
    Q_OBJECT

public:
    explicit ImportBitsWizard(QString fileName, QWidget *parent = nullptr);

    ~ImportBitsWizard();

    QSharedPointer<BitContainer> getImportedBitContainer();

public slots:
    void adjustMaxBits();

private:
    Ui::ImportBitsWizard *ui;
    QString m_fileName;
    double m_fileSizeKb;
};

#endif // IMPORTBITSWIZARD_H
