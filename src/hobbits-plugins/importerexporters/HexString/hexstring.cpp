#include "hexstring.h"
#include "hexstringimportform.h"
#include "parametereditorfileselect.h"
#include "settingsmanager.h"
#include <QFileDialog>

HexString::HexString()
{

    QList<ParameterDelegate::ParameterInfo> importInfos = {
        {"filename", QJsonValue::String, true},
        {"hex_string", QJsonValue::String, true},
        {"repeats", QJsonValue::Double, true}
    };
    m_importDelegate = QSharedPointer<ParameterDelegateUi>(
                new ParameterDelegateUi(
                    importInfos,
                    [](const QJsonObject &parameters) {
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
                    }));


    QList<ParameterDelegate::ParameterInfo> exportInfos = {
        {"filename", QJsonValue::String, false}
    };
    m_exportDelegate = QSharedPointer<ParameterDelegateUi>(
                new ParameterDelegateUi(
                    exportInfos,
                    [](const QJsonObject &parameters) {
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
                    }));
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

QSharedPointer<ImportResult> HexString::importBits(QJsonObject parameters,
                                                   QSharedPointer<PluginActionProgress> progress)
{
    Q_UNUSED(progress)
    QSharedPointer<BitContainer> container;

    auto importer = QSharedPointer<HexStringImportForm>(new HexStringImportForm());

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
                                                   QJsonObject parameters,
                                                   QSharedPointer<PluginActionProgress> progress)
{
    Q_UNUSED(progress)
    if (m_exportDelegate->validate(parameters)) {
        ExportResult::error(QString("Invalid parameters passed to '%1'").arg(name()));
    }

    QString fileName;
    fileName = parameters.value("filename").toString();
    QFile file(fileName);

    if (!file.open(QIODevice::Truncate | QIODevice::WriteOnly)) {
        ExportResult::error(QString("Failed to open export file: '%1'").arg(fileName));
    }
    SettingsManager::setPrivateSetting(
            SettingsManager::LAST_IMPORT_EXPORT_PATH_KEY,
            QFileInfo(file).dir().path());

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
