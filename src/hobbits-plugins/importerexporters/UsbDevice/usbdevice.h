#pragma once

#include "importexportinterface.h"
#include "parameterdelegate.h"
#include <libusb.h>

class USBDevice : public QObject, ImporterExporterInterface
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID "hobbits.ImporterExporterInterface.USBDevice")
    Q_INTERFACES(ImporterExporterInterface)

public:
    USBDevice();

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

    int setupLibusb();
    void exitLibusb(bool closeDevice);

private:
    QSharedPointer<ParameterDelegate> m_importDelegate;
    QSharedPointer<ParameterDelegate> m_exportDelegate;
    //the pointer to the libusb device selected
    libusb_device *m_dev;
    //the pointer to the device list generated
    libusb_device **m_devs;
    //the pointer to the configuration descriptor generated
    libusb_config_descriptor *m_config;
    // the pointer to the libusb context for the active libusb session
    libusb_context *m_ctx;
    //the handle of the device used for transfers and device interactions
    libusb_device_handle *m_handle;
    //the number of the device selected
    int m_deviceNum;
    //the number of the interface selected
    int m_interfaceNum;
    //the number of the alternate setting selected
    int m_altSetNum;
    //number of the endpoint selected
    int m_endpointNum;
    //the address of the endpoint selected
    unsigned char m_endpoint;
};

