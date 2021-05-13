#include "displayprint.h"
#include "displayprintexportform.h"
#include <QScrollBar>
#include <QPainter>
#include "displayresult.h"

DisplayPrint::DisplayPrint()
{
    QList<ParameterDelegate::ParameterInfo> exportInfos = {
        {"plugin_name", ParameterDelegate::ParameterType::String},
        {"image_width", ParameterDelegate::ParameterType::Integer},
        {"image_height", ParameterDelegate::ParameterType::Integer},
        {"image_filename", ParameterDelegate::ParameterType::String},
        {"display_params", ParameterDelegate::ParameterType::Object}
    };

    m_exportDelegate = ParameterDelegate::create(
                    exportInfos,
                    [this](const Parameters &parameters) {
                        QString pluginName = parameters.value("plugin_name").toString();
                        return QString("Export %1 Image").arg(pluginName);
                    },
                    [](QSharedPointer<ParameterDelegate> delegate, QSize size) {
                            Q_UNUSED(size)
                            return new DisplayPrintExportForm(delegate);
                    });
}

ImporterExporterInterface* DisplayPrint::createDefaultImporterExporter()
{
    return new DisplayPrint();
}

QString DisplayPrint::name()
{
    return "Display Print";
}

QString DisplayPrint::description()
{
    return "Saves a Display Plugin's display to an image";
}

QStringList DisplayPrint::tags()
{
    return {"Generic"};
}

bool DisplayPrint::canExport()
{
    return true;
}

bool DisplayPrint::canImport()
{
    return false;
}

QSharedPointer<ParameterDelegate>  DisplayPrint::importParameterDelegate()
{
    return nullptr;
}

QSharedPointer<ParameterDelegate>  DisplayPrint::exportParameterDelegate()
{
    return m_exportDelegate;
}

QSharedPointer<ImportResult> DisplayPrint::importBits(const Parameters &parameters,
                                                      QSharedPointer<PluginActionProgress> progress)
{
    Q_UNUSED(parameters)
    Q_UNUSED(progress)
    return ImportResult::error("Import not implemented");
}

QSharedPointer<ExportResult> DisplayPrint::exportBits(QSharedPointer<const BitContainer> container,
                                                      const Parameters &parameters,
                                                      QSharedPointer<PluginActionProgress> progress)
{
    QStringList invalidations = m_exportDelegate->validate(parameters);
    if (!invalidations.isEmpty()) {
        return ExportResult::error(QString("Invalid parameters passed to %1:\n%2").arg(name()).arg(invalidations.join("\n")));
    }

    int imageWidth = parameters.value("image_width").toInt();
    int imageHeight = parameters.value("image_height").toInt();

    auto pluginManager = DisplayPrintExportForm::loadUpPluginManager();
    auto displayPlugin = pluginManager->getDisplay(parameters.value("plugin_name").toString());
    if (displayPlugin.isNull()) {
        return ExportResult::error(QString("Failed to load display '%1'").arg(parameters.value("plugin_name").toString()));
    }

    QSharedPointer<BitContainerManager> containerManager(new BitContainerManager());
    QSharedPointer<DisplayHandle> displayHandle(new DisplayHandle(containerManager));
    displayPlugin->setDisplayHandle(displayHandle);

    auto noConstContainer = BitContainer::create(container->bits(), container->info());
    containerManager->addContainer(noConstContainer);
    containerManager->selectContainer(noConstContainer);

    Parameters displayParams(parameters.value("display_params").toObject());

    auto display = displayPlugin->renderDisplay(QSize(imageWidth, imageHeight), displayParams, progress);
    auto overlay = displayPlugin->renderOverlay(QSize(imageWidth, imageHeight), displayParams);

    if (!display->errorString().isEmpty()) {
        return ExportResult::error(display->errorString());
    }
    if (!overlay->errorString().isEmpty()) {
        return ExportResult::error(overlay->errorString());
    }

    QPixmap pixmap(imageWidth, imageHeight);
    QPainter painter(&pixmap);
    painter.drawImage(0, 0, display->getImage());
    painter.drawImage(0, 0, overlay->getImage());

    QFile file(parameters.value("image_filename").toString());
    if (!file.open(QFile::WriteOnly | QFile::Truncate)) {
        return ExportResult::error(QString("Failed to open file for writing '%1'").arg(file.fileName()));
    }
    pixmap.save(&file, "PNG");

    return ExportResult::result(parameters);
}
