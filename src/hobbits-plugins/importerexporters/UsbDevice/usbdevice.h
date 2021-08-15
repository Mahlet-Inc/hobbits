#pragma once

#include "importexportinterface.h"
#include "parameterdelegate.h"
#include <libusb-1.0/libusb.h>

class UsbDevice : public QObject, ImporterExporterInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "hobbits.ImporterExporterInterface.UsbDevice")
    Q_INTERFACES(ImporterExporterInterface)

public:
    UsbDevice();

    ImporterExporterInterface* createDefaultImporterExporter() override;

    QString name() override;
    QString description() override;
    QStringList tags() override;

    bool canExport() override;
    bool canImport() override;

    virtual QSharedPointer<ParameterDelegate> importParameterDelegate() override;
    virtual QSharedPointer<ParameterDelegate> exportParameterDelegate() override;

    QSharedPointer<ImportResult> importBits(const Parameters &parameters,
                                            QSharedPointer<PluginActionProgress> progress) override;


    QSharedPointer<ExportResult> exportBits(QSharedPointer<const BitContainer> container,
                                            const Parameters &parameters,
                                            QSharedPointer<PluginActionProgress> progress) override;

   struct usbParams setupLibusb(usbParams params);
    QSharedPointer<ImportResult> returnError(int errorCode);
   struct usbParams exitLibusb(bool closeDevice, usbParams params);

private:
    QSharedPointer<ParameterDelegate> m_importDelegate;
    QSharedPointer<ParameterDelegate> m_exportDelegate;
   
};

struct usbParams
{
     //the pointer to the libusb device selected
    libusb_device *dev;
    //the pointer to the device list generated
    libusb_device **devs;
    //the pointer to the configuration descriptor generated
    libusb_config_descriptor *config;
    // the pointer to the libusb context for the active libusb session
    libusb_context *ctx;
    //the handle of the device used for transfers and device interactions
    libusb_device_handle *handle;
    //the number of the device selected
    int deviceNum;
    //the number of the interface selected
    int interfaceNum;
    //the number of the alternate setting selected
    int altSetNum;
    //number of the endpoint selected
    int endpointNum;
    //the address of the endpoint selected
    unsigned char endpoint;
    //any possible error codes passed
    int errorCode;
};
