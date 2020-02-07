#ifndef %{JS: '%{ClassName}'.toUpperCase()}_H
#define %{JS: '%{ClassName}'.toUpperCase()}_H

#include "importexportinterface.h"

class %{ClassName} : public QObject, ImportExportInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "hobbits.ImportExportInterface.%{ClassName}")
    Q_INTERFACES(ImportExportInterface)

public:
    %{ClassName}();
    ~%{ClassName}();

    ImportExportInterface* createDefaultImporterExporter() override;

    QString getName() override;

    bool canExport() override;
    bool canImport() override;

    QSharedPointer<BitContainer> importBits(QMap<QString, QString> args, QWidget *parent) override;
    void exportBits(
            QSharedPointer<const BitContainer> container,
            QMap<QString, QString> args,
            QWidget *parent) override;

private:
};

#endif // %{JS: '%{ClassName}'.toUpperCase()}_H
