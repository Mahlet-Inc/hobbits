#pragma once

#include "importexportinterface.h"
#include "parameterdelegate.h"
#include <libusb-1.0/libusb.h>

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
    libusb_device *m_dev;
    libusb_device **m_devs;
    libusb_config_descriptor *m_config;
    libusb_context *m_ctx;
    unsigned char m_endpoint;
    libusb_device_handle *m_handle;
    int m_deviceNum;
    int m_interfaceNum;
    int m_altSetNum;
    int m_endpointNum;
};

