#pragma once

#include "abstractparametereditor.h"
#include "parameterhelper.h"
#include <libusb-1.0/libusb.h>

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


public slots:

    
    void setInterfaces(QString device);
    void setAltSet(QString interface);
    void setEndpoint(QString altSet);

private:

    void initLibusb(libusb_context *ctx);
    QStringList getUsbDevices();
    void updateSelector(QComboBox *selector, QStringList items);
    void clearSelectors(QComboBox *boxes[]);

    void previewBitsImpl(QSharedPointer<BitContainerPreview> container,
                             QSharedPointer<PluginActionProgress> progress) override;
    void previewBitsUiImpl(QSharedPointer<BitContainerPreview> container) override;

    Ui::USBDeviceImportEditor *ui;
    QSharedPointer<ParameterHelper> m_paramHelper;
    QStringList m_devices;
    QStringList m_interfaces;
    QStringList m_altSets;
    QStringList m_endpoints;
    libusb_device *m_dev;
    libusb_device **m_devs;
    libusb_config_descriptor *m_config;
    int m_interfaceNum;
    int m_altSetNum;
    int m_endpointNum;
    unsigned char m_endpoint;
};



