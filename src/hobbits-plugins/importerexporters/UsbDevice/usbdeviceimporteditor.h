#pragma once

#include "abstractparametereditor.h"
#include "parameterhelper.h"
#include <libusb-1.0/libusb.h>
#include "importresult.h"

namespace Ui
{
class USBDeviceImportEditor;
}

class USBDeviceImportEditor : public AbstractParameterEditor
{
    Q_OBJECT

public:
    USBDeviceImportEditor(QSharedPointer<ParameterDelegate> delegate);
    ~USBDeviceImportEditor() override;

    QString title() override;

    bool setParameters(const Parameters &parameters) override;
    Parameters parameters() override;
    static QSharedPointer<ImportResult> importData(const Parameters &parameters, QSharedPointer<PluginActionProgress> progress);

    
    uint8_t m_transferType;
    uint16_t m_transferSize;
    int m_interfaceNum;
    int m_altSetNum;
    int m_endpointNum;
    unsigned char m_endpoint;
public slots:

    void populateInterfaces(QString device);
    void populateAltSet(QString interface);
    void populateEndpoint(QString altSet);
    void configureEndpoint(QString endpoint);

private:

    void initLibusb();
    QStringList getUsbDevices();
    void updateSelector(QComboBox *selector, QStringList items);
    void clearSelectors(QComboBox *boxes[]);

    Ui::USBDeviceImportEditor *ui;
    QSharedPointer<ParameterHelper> m_paramHelper;
    
    QStringList m_devices;
    QStringList m_interfaces;
    QStringList m_altSets;
    QStringList m_endpoints;
    libusb_device *m_dev;
    libusb_device **m_devs;
    libusb_config_descriptor *m_config;
    libusb_context *m_ctx;
    int m_deviceNum;
    int m_cnt;
    QString m_device;
};



