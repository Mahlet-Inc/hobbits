#include "displayprint.h"
#include "displayprintexportform.h"
#include <QScrollBar>

DisplayPrint::DisplayPrint()
{
    QList<ParameterDelegate::ParameterInfo> exportInfos = {
        {"plugin_name", QJsonValue::String},
        {"image_width", QJsonValue::Double},
        {"image_height", QJsonValue::Double},
        {"image_filename", QJsonValue::String}
    };

    m_exportDelegate = ParameterDelegate::create(
                    exportInfos,
                    [this](const QJsonObject &parameters) {
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

QSharedPointer<ImportResult> DisplayPrint::importBits(QJsonObject parameters,
                                                      QSharedPointer<PluginActionProgress> progress)
{
    Q_UNUSED(parameters)
    Q_UNUSED(progress)
    return ImportResult::error("Import not implemented");
}

QSharedPointer<ExportResult> DisplayPrint::exportBits(QSharedPointer<const BitContainer> container,
                                                      QJsonObject parameters,
                                                      QSharedPointer<PluginActionProgress> progress)
{
    if (!m_exportDelegate->validate(parameters)) {
        return ExportResult::error(QString("Invalid parameters passed to %1").arg(name()));
    }

    auto pluginManager = DisplayPrintExportForm::loadUpPluginManager();
    auto displayPlugin = pluginManager->getDisplay(parameters.value("plugin_name").toString());
    if (displayPlugin.isNull()) {
        return ExportResult::error(QString("Failed to load display '%1'").arg(parameters.value("plugin_name").toString()));
    }

    QSharedPointer<BitContainerManager> containerManager(new BitContainerManager());
    QScopedPointer<QScrollBar> vScrollDummy(new QScrollBar());
    QScopedPointer<QScrollBar> hScrollDummy(new QScrollBar());

    QSharedPointer<DisplayHandle> displayHandle(new DisplayHandle(containerManager, vScrollDummy.data(), hScrollDummy.data()));

    //TODO: make this less hacky
    auto noConstContainer = BitContainer::create(container->bits(), container->info());

    auto display = displayPlugin->display(displayHandle);
    display->setGeometry(0, 0, parameters.value("image_width").toInt(), parameters.value("image_height").toInt());
    containerManager->addContainer(noConstContainer);
    containerManager->selectContainer(noConstContainer);

    QPixmap pixmap(parameters.value("image_width").toInt(), parameters.value("image_height").toInt());
    display->render(&pixmap);

    QFile file(parameters.value("image_filename").toString());
    if (!file.open(QFile::WriteOnly | QFile::Truncate)) {
        return ExportResult::error(QString("Failed to open file for writing '%1'").arg(file.fileName()));
    }
    pixmap.save(&file, "PNG");

    return ExportResult::result(parameters);
}
