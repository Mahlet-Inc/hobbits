#include "bitcontainerdata.h"
#include "parametereditorfileselect.h"
#include "settingsmanager.h"

BitContainerData::BitContainerData() 
{
    QList<ParameterDelegate::ParameterInfo> infos = {
        {"filename", ParameterDelegate::ParameterType::String}
    };

    m_importDelegate = ParameterDelegate::create(
                    infos,
                    [this](const Parameters &parameters) {
                        if (parameters.contains("filename")) {
                            return QString("Import container from %1").arg(parameters.value("filename").toString());
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
                    [this](const Parameters &parameters) {
                        if (parameters.contains("filename")) {
                            return QString("Export container to %1").arg(parameters.value("filename").toString());
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

ImporterExporterInterface* BitContainerData::createDefaultImporterExporter()
{
    return new BitContainerData();
}

QString BitContainerData::name()
{
    return "Hobbits Bit Container";
}

QString BitContainerData::description()
{
    return "Imports and exports Hobbits bit containers with their metadata";
}

QStringList BitContainerData::tags()
{
    return {"Generic"};
}

bool BitContainerData::canExport()
{
    return true;
}

bool BitContainerData::canImport()
{
    return true;
}

QSharedPointer<ParameterDelegate>  BitContainerData::importParameterDelegate()
{
    return m_importDelegate;
}

QSharedPointer<ParameterDelegate>  BitContainerData::exportParameterDelegate()
{
    return m_exportDelegate;
}

static const QString LAST_BIT_CONTAINER_IMPORT_EXPORT = "last_bit_container_import_export";

QSharedPointer<ImportResult> BitContainerData::importBits(const Parameters &parameters,
                                                      QSharedPointer<PluginActionProgress> progress)
{
    Q_UNUSED(progress)
    QStringList invalidations = m_importDelegate->validate(parameters);
    if (!invalidations.isEmpty()) {
        return ImportResult::error(QString("Invalid parameters passed to %1:\n%2").arg(name()).arg(invalidations.join("\n")));
    }
    QString fileName = parameters.value("filename").toString();
    if (fileName.isEmpty()) {
        return ImportResult::error("No file selected for import");
    }
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        return ImportResult::error(QString("Failed to open file for import: '%1'").arg(fileName));
    }
    SettingsManager::setPrivateSetting(
            LAST_BIT_CONTAINER_IMPORT_EXPORT,
            QFileInfo(file).dir().path());
    
    QDataStream stream(&file);
    QSharedPointer<BitContainer> container = BitContainer::deserialize(stream);
    if (container.isNull()) {
        return ImportResult::error(QString("Failed to load Bit Container from %1\n\nIs it a valid Bit Container file?").arg(fileName));
    }
    container->setName(QFileInfo(file).fileName());

    return ImportResult::result(container, parameters);
}

QSharedPointer<ExportResult> BitContainerData::exportBits(QSharedPointer<const BitContainer> container,
                                                      const Parameters &parameters,
                                                      QSharedPointer<PluginActionProgress> progress)
{
    QStringList invalidations = m_exportDelegate->validate(parameters);
    if (!invalidations.isEmpty()) {
        return ExportResult::error(QString("Invalid parameters passed to %1:\n%2").arg(name()).arg(invalidations.join("\n")));
    }
    progress->setProgressPercent(10);

    QString fileName = parameters.value("filename").toString();
    if (fileName.isEmpty()) {
        return ExportResult::error("No file selected for export");
    }

    QFile file(fileName);
    if (!file.open(QIODevice::Truncate | QIODevice::WriteOnly)) {
        return ExportResult::error(QString("Failed to open export bit file: '%1'").arg(fileName));
    }
    SettingsManager::setPrivateSetting(
            SettingsManager::LAST_IMPORT_EXPORT_PATH_KEY,
            QFileInfo(file).dir().path());

    QDataStream stream(&file);
    container->serialize(stream);
    file.close();

    progress->setProgressPercent(90);

    return ExportResult::result(parameters);
}
