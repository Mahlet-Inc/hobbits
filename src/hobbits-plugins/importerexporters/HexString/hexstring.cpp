#include "hexstring.h"
#include "hexstringimportform.h"
#include "parametereditorfileselect.h"
#include "settingsmanager.h"
#include <QFileDialog>

HexString::HexString()
{

    QList<ParameterDelegate::ParameterInfo> importInfos = {
        {"filename", ParameterDelegate::ParameterType::String, true},
        {"hex_string", ParameterDelegate::ParameterType::String, true},
        {"repeats", ParameterDelegate::ParameterType::Integer, true}
    };
    m_importDelegate = ParameterDelegate::create(
                    importInfos,
                    [](const Parameters &parameters) {
                        if (parameters.contains("filename")) {
                            return QString("Import Hex String from %1").arg(parameters.value("filename").toString());
                        }
                        else if (parameters.contains("hex_string")) {
                            QString hexString = parameters.value("hex_string").toString();
                            if (hexString.size() > 16) {
                                hexString.truncate(12);
                                hexString += "...";
                            }
                            return QString("Import Hex '%1'").arg(hexString);
                        }
                        else {
                            return QString();
                        }
                    },
                    [](QSharedPointer<ParameterDelegate> delegate, QSize size) {
                        Q_UNUSED(size)
                        Q_UNUSED(delegate)
                        return new HexStringImportForm();
                    });


    QList<ParameterDelegate::ParameterInfo> exportInfos = {
        {"filename", ParameterDelegate::ParameterType::String, false}
    };
    m_exportDelegate = ParameterDelegate::create(
                    exportInfos,
                    [](const Parameters &parameters) {
                        if (parameters.contains("filename")) {
                            return QString("Export Hex String to %1").arg(parameters.value("filename").toString());
                        }
                        else {
                            return QString();
                        }
                    },
                    [](QSharedPointer<ParameterDelegate> delegate, QSize size) {
                        Q_UNUSED(size)
                        Q_UNUSED(delegate)
                        return new ParameterEditorFileSelect(QFileDialog::AcceptSave);
                    });
}

ImporterExporterInterface* HexString::createDefaultImporterExporter()
{
    return new HexString();
}

QString HexString::name()
{
    return "Hex String";
}

QString HexString::description()
{
    return "ASCII-encoded Hex String importing and exporting";
}

QStringList HexString::tags()
{
    return {"Generic"};
}

bool HexString::canExport()
{
    return true;
}

bool HexString::canImport()
{
    return true;
}

QSharedPointer<ParameterDelegate> HexString::importParameterDelegate()
{
    return m_importDelegate;
}

QSharedPointer<ParameterDelegate> HexString::exportParameterDelegate()
{
    return m_exportDelegate;
}

QSharedPointer<ImportResult> HexString::importBits(const Parameters &parameters,
                                                   QSharedPointer<PluginActionProgress> progress)
{
    Q_UNUSED(progress)
    QSharedPointer<BitContainer> container;

    if (parameters.contains("filename")) {
        QString fileName = parameters.value("filename").toString();
        return HexStringImportForm::importFromFile(fileName);
    }
    else if (parameters.contains("hex_string")) {
        QString hexString = parameters.value("hex_string").toString();
        int repeats = 1;
        if (parameters.contains("repeats")) {
            repeats = parameters.value("repeats").toInt();
        }
        return HexStringImportForm::importFromHexString(hexString, repeats);
    }
    else {
       return ImportResult::error("Hex String import requires either a filename or a hex string");
    }
}

QSharedPointer<ExportResult> HexString::exportBits(QSharedPointer<const BitContainer> container,
                                                   const Parameters &parameters,
                                                   QSharedPointer<PluginActionProgress> progress)
{
    Q_UNUSED(progress)
    QStringList invalidations = m_exportDelegate->validate(parameters);
    if (!invalidations.isEmpty()) {
        return ExportResult::error(QString("Invalid parameters passed to %1:\n%2").arg(name()).arg(invalidations.join("\n")));
    }

    QString fileName;
    fileName = parameters.value("filename").toString();
    QFile file(fileName);

    if (!file.open(QIODevice::Truncate | QIODevice::WriteOnly)) {
        ExportResult::error(QString("Failed to open export file: '%1'").arg(fileName));
    }

    qint64 byteLen = container->bits()->sizeInBytes();
    qint64 offset = 0;
    while (offset < byteLen) {
        QByteArray bytes = container->bits()->readBytes(offset, qMin(byteLen - offset, 100000ll)).toHex(' ');
        if (bytes.size() < 1) {
            return ExportResult::error("Failed to export container as hex string - failure reading from container");
        }
        qint64 written = file.write(bytes.data());
        if (written < bytes.size()) {
            return ExportResult::error("Failed to export container as hex string - failure writing hex string to file");
        }
        offset += bytes.size();
    }
    file.close();

    return ExportResult::result(parameters);
}
