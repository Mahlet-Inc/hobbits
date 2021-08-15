#pragma once

#include "abstractparametereditor.h"
#include "parameterhelper.h"
#include <libusb-1.0/libusb.h>
#include "importresult.h"

namespace Ui
{
class UsbDeviceImportEditor;
}

class UsbDeviceImportEditor : public AbstractParameterEditor
{
    Q_OBJECT

public:
    UsbDeviceImportEditor(QSharedPointer<ParameterDelegate> delegate);
    ~UsbDeviceImportEditor() override;

    QString title() override;

    bool setParameters(const Parameters &parameters) override;
    Parameters parameters() override;

    //the type of the transfer the endpoint can perform
    uint8_t m_transferType;
    // the size of the transfer the endpoint can perform
    uint16_t m_transferSize;
    //the number of the interface selected
    int m_interfaceNum;
    //the number of the alternate setting selected
    int m_altSetNum;
    //number of the endpoint selected
    int m_endpointNum;
    //the address of the endpoint selected
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
    QString addLeadingZeros(std::string str);
    QStringList getVendorAndProduct(QString idVendor, QString idProduct);
    Ui::UsbDeviceImportEditor *ui;
    QSharedPointer<ParameterHelper> m_paramHelper;
    
    //the list of different devices
    QStringList m_devices;
    //the list of possible interfaces to select from
    QStringList m_interfaces;
    //the list of possible alternate settings to select from 
    QStringList m_altSets;
    //the list of possible endpoints to select from
    QStringList m_endpoints;
    //the pointer to the libusb device selected
    libusb_device *m_dev;
    //the pointer to the device list generated
    libusb_device **m_devs;
    //the pointer to the configuration descriptor generated
    libusb_config_descriptor *m_config;
    // the pointer to the libusb context for the active libusb session
    libusb_context *m_ctx;
    //the number of the device selected
    int m_deviceNum;
    // the number of devices in the list
    int m_cnt;
    // the string descriptor of the device selected
    QString m_device;
};
