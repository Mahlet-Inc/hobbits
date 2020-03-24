#include "hexstring.h"
#include "hexstringimporter.h"
#include "settingsmanager.h"
#include <QFileDialog>

HexString::HexString()
{

}

HexString::~HexString()
{
}

ImportExportInterface* HexString::createDefaultImporterExporter()
{
    return new HexString();
}

QString HexString::getName()
{
    return "Hex String";
}

bool HexString::canExport()
{
    return true;
}

bool HexString::canImport()
{
    return true;
}

QSharedPointer<BitContainer> HexString::importBits(QMap<QString, QString> args, QWidget *parent)
{
    Q_UNUSED(args)

    QSharedPointer<BitContainer> container;

    HexStringImporter *importer = new HexStringImporter(parent);

    if (importer->exec()) {
        container = importer->getContainer();
        delete importer;
    }

    return container;
}

void HexString::exportBits(QSharedPointer<const BitContainer> container, QMap<QString, QString> args, QWidget *parent)
{
    Q_UNUSED(parent)
    Q_UNUSED(args)

    QString fileName;

    if (args.contains("filename")) {
        fileName = args.value("filename");
    }
    else {
        fileName = QFileDialog::getSaveFileName(
                parent,
                tr("Export Bits"),
                SettingsManager::getInstance().getPrivateSetting(SettingsData::LAST_IMPORT_EXPORT_PATH_KEY).toString(),
                tr("All Files (*)"));
    }
    QFile file(fileName);
    if (!file.open(QIODevice::Truncate | QIODevice::WriteOnly)) {
        return;
    }
    SettingsManager::getInstance().setPrivateSetting(
            SettingsData::LAST_IMPORT_EXPORT_PATH_KEY,
            QFileInfo(file).dir().path());

    int len = container->bits()->getPreviewSize() / 8 + (container->bits()->getPreviewSize() % 8 ? 1 : 0);
    QByteArray bytes = container->bits()->getPreviewBytes().mid(0, len).toHex(' ');
    file.write(bytes.data());
    file.close();
}
