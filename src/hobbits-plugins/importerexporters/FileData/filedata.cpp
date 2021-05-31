#include "filedata.h"
#include "settingsmanager.h"
#include <QFileDialog>
#include <QMessageBox>
#include "parametereditorfileselect.h"
#include <QVBoxLayout>
#include <QLabel>
#include "parametereditordialog.h"

FileData::FileData()
{
    QList<ParameterDelegate::ParameterInfo> infos = {
        {"filename", ParameterDelegate::ParameterType::String, false}
    };

    m_importDelegate = ParameterDelegate::create(
                    infos,
                    [](const Parameters &parameters) {
                        if (parameters.contains("filename")) {
                            return QString("Import from %1").arg(parameters.value("filename").toString());
                        }
                        else {
                            return QString();
                        }
                    },
                    [](QSharedPointer<ParameterDelegate> delegate, QSize size) {
                        Q_UNUSED(size)
                        Q_UNUSED(delegate)
                        return new ParameterEditorFileSelect(QFileDialog::AcceptOpen);
                    });

    m_exportDelegate = ParameterDelegate::create(
                    infos,
                    [](const Parameters &parameters) {
                        if (parameters.contains("filename")) {
                            return QString("Export to %1").arg(parameters.value("filename").toString());
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

FileData::~FileData()
{
}

ImporterExporterInterface* FileData::createDefaultImporterExporter()
{
    return new FileData();
}

QString FileData::name()
{
    return "File Data";
}

QString FileData::description()
{
    return "Raw file data importing and exporting";
}

QStringList FileData::tags()
{
    return {"Generic"};
}

bool FileData::canExport()
{
    return true;
}

bool FileData::canImport()
{
    return true;
}

QSharedPointer<ParameterDelegate> FileData::importParameterDelegate()
{
    return m_importDelegate;
}

QSharedPointer<ParameterDelegate> FileData::exportParameterDelegate()
{
    return m_exportDelegate;
}

QSharedPointer<ImportResult> FileData::importBits(const Parameters &parameters,
                                                  QSharedPointer<PluginActionProgress> progress)
{
    Q_UNUSED(progress)
    QString fileName = parameters.value("filename").toString();
    if (fileName.isEmpty()) {
        return ImportResult::error("No file selected for import");
    }
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        return ImportResult::error(QString("Failed to open file for import: '%1'").arg(fileName));
    }

    QSharedPointer<BitContainer> container = BitContainer::create(&file);
    container->setName(QFileInfo(file).fileName());

    return ImportResult::result(container, parameters);
}

QSharedPointer<ExportResult> FileData::exportBits(QSharedPointer<const BitContainer> container,
                                                  const Parameters &parameters,
                                                  QSharedPointer<PluginActionProgress> progress)
{
    progress->setProgressPercent(10);
    QString fileName = parameters.value("filename").toString();
    if (fileName.isEmpty()) {
        return ExportResult::error("No file selected for export");
    }

    QFile file(fileName);
    if (!file.open(QIODevice::Truncate | QIODevice::WriteOnly)) {
        return ExportResult::error(QString("Failed to open export bit file: '%1'").arg(fileName));
    }

    container->bits()->writeTo(&file);
    file.close();
    progress->setProgressPercent(90);

    return ExportResult::result(parameters);
}
