#include "usbdevice.h"
#include "usbdeviceimporteditor.h"
#include <libusb-1.0/libusb.h>


USBDevice::USBDevice() 
{
    QList<ParameterDelegate::ParameterInfo> importInfos = {
        // TODO: add parameters like {"myparametername", ParameterDelegate::ParameterType::Decimal}
    };

    m_importDelegate = ParameterDelegate::create(
                    importInfos,
                    [this](const Parameters &parameters) {
                        // TODO: use parameters to describe action better
                        return QString("%1 Import").arg(this->name());
                    },
                    [](QSharedPointer<ParameterDelegate> delegate, QSize size) {
                        Q_UNUSED(size)
                        return new USBDeviceImportEditor(delegate);
                    });

    QList<ParameterDelegate::ParameterInfo> exportInfos = {
        // TODO: add parameters like {"myparametername", ParameterDelegate::ParameterType::Decimal}
    };

     m_exportDelegate = nullptr;
    // ParameterDelegate::create(
    //                 exportInfos,
    //                 [this](const Parameters &parameters) {
    //                     // TODO: use parameters to describe action better
    //                     return QString("%1 Export").arg(this->name());
    //                 },
    //                 [](QSharedPointer<ParameterDelegate> delegate, QSize size) {
    //                     Q_UNUSED(size)
    //                     return new USBDeviceExportEditor(delegate);
    //                 });
}

ImporterExporterInterface* USBDevice::createDefaultImporterExporter()
{
    return new USBDevice();
}

QString USBDevice::name()
{
    return "USB Device";
}

QString USBDevice::description()
{
    // TODO: create actual description
    return "Describes what USBDevice does";
}

QStringList USBDevice::tags()
{
    // TODO: add relevant tags
    return {"Generic"};
}

bool USBDevice::canExport()
{
    return false;
}

bool USBDevice::canImport()
{
    return true;
}

QSharedPointer<ParameterDelegate>  USBDevice::importParameterDelegate()
{
    return m_importDelegate;
}

QSharedPointer<ParameterDelegate>  USBDevice::exportParameterDelegate()
{
    return m_exportDelegate;
}

QSharedPointer<ImportResult> USBDevice::importBits(const Parameters &parameters,
                                                      QSharedPointer<PluginActionProgress> progress)
{
    QStringList invalidations = m_importDelegate->validate(parameters);
    if (!invalidations.isEmpty()) {
        return ImportResult::error(QString("Invalid parameters passed to %1:\n%2").arg(name()).arg(invalidations.join("\n")));
    }

    return ImportResult::error("Import not implemented");
}

QSharedPointer<ExportResult> USBDevice::exportBits(QSharedPointer<const BitContainer> container,
                                                      const Parameters &parameters,
                                                      QSharedPointer<PluginActionProgress> progress)
{
    return ExportResult::error("Export not implemented");
}
