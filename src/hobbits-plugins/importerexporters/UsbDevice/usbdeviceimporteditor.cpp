#include "usbdeviceimporteditor.h"
#include "ui_usbdeviceimporteditor.h"
#include <libusb-1.0/libusb.h>
#include "subprocess.hpp"

namespace sp = subprocess;

USBDeviceImportEditor::USBDeviceImportEditor(QSharedPointer<ParameterDelegate> delegate):
    ui(new Ui::USBDeviceImportEditor()),
    m_paramHelper(new ParameterHelper(delegate))
  
{
    ui->setupUi(this);

    // TODO: Correlate parameters in given delegate to form fields
    // Ex 1.
    // m_paramHelper->addSpinBoxIntParameter("myparametername", ui->spinBox);
    // Ex 2.
    // m_paramHelper->addParameter("otherparameter", [this](QJsonValue value) {
    //     // set the value in the editor
    //     ui->spinBox->setValue(value.toInt());
    //     return true; // return false if unable to set the value in the editor
    // }, [this]() {
    //     // get the QJsonValue from the editor
    //     return QJsonValue(ui->spinBox->value());
    // });

    libusb_context *ctx;
    initLibusb(ctx);

    m_paramHelper->addComboBoxParameter("DeviceNum", ui->DeviceSelector);
    m_paramHelper->addComboBoxParameter("InterfaceNum", ui->InterfaceSelector);
    m_paramHelper->addComboBoxParameter("AltSetNum", ui->AltSetSelector);
    m_paramHelper->addComboBoxParameter("EndpointNum", ui->InterfaceSelector);
    m_paramHelper->addSpinBoxIntParameter("TransferNum", ui->TransferNum);
    m_paramHelper->addSpinBoxIntParameter("TransferDelay", ui->TransferDelay);
    m_paramHelper->addSpinBoxIntParameter("TransferTimeout", ui->TransferTimeout);

    m_devices = this->getUsbDevices();
    this->updateSelector(ui->DeviceSelector, m_devices);
    connect(ui->DeviceSelector, SIGNAL(currentTextChanged(const QString &)), this, SLOT(setInterfaces(const QString &)));
    connect(ui->InterfaceSelector, SIGNAL(currentTextChanged(const QString &)), this, SLOT(setAltSet(const QString &)));
    connect(ui->AltSetSelector, SIGNAL(currentTextChanged(const QString &)), this, SLOT(setEndpoint(const QString &)));

}

USBDeviceImportEditor::~USBDeviceImportEditor()
{
    delete ui;
}

QString USBDeviceImportEditor::title()
{
    return "Select USB Device & Configurations to Read From";
}

Parameters USBDeviceImportEditor::parameters()
{
    return m_paramHelper->getParametersFromUi();
}

bool USBDeviceImportEditor::setParameters(const Parameters &parameters)
{
    return m_paramHelper->applyParametersToUi(parameters);
}

void USBDeviceImportEditor::previewBitsImpl(QSharedPointer<BitContainerPreview> container,
                                      QSharedPointer<PluginActionProgress> progress)
{
    // TODO: (Optional) Preview the currently active BitContainer (preprocess it, enrich it, etc)
}

void USBDeviceImportEditor::previewBitsUiImpl(QSharedPointer<BitContainerPreview> container)
{
    // TODO: (Optional) Update UI elements to account for preprocessing in previewBitsImpl and/or other metadata
}

void USBDeviceImportEditor::initLibusb(libusb_context *ctx){
    m_devices.clear();
    int r = libusb_init(&ctx); //initialize a library session
	if(r < 0) {
        std::runtime_error libusb_init_error("Error while trying to initialize Libusb");
		throw libusb_init_error;
	}
    libusb_set_option(ctx, LIBUSB_OPTION_LOG_LEVEL, 4 );
    int cnt = libusb_get_device_list(ctx, &m_devs);
    if (cnt < 0){
        std::runtime_error libusb_device_list_error("Error getting device list");
        throw libusb_device_list_error;
    }

}
    
QStringList USBDeviceImportEditor::getUsbDevices(){
    auto obuf = sp::check_output({"lsusb"});
    QString output = obuf.buf.data();
    QStringList devices = output.split("\n");
    devices.removeLast();
    return devices;
}

void USBDeviceImportEditor::setInterfaces(QString device){
    
   int deviceNum;
   m_interfaces.clear();
   if(m_devices.contains(device) == true){

        deviceNum = m_devices.indexOf(device);
        m_dev = m_devs[deviceNum];
        libusb_config_descriptor *config;
        libusb_get_active_config_descriptor(m_dev, &config);
        
        const libusb_interface *inter;

        for(int i  = 0; i < (int)config->bNumInterfaces; i++){
            inter = &config->interface[i];
            int altSets = inter->num_altsetting;
            QString interface = "Interface " + QString::number(i) + ": Number of Alternate Settings: " + QString::number(altSets);
            m_interfaces.append(interface);
        }
        m_altSets.clear();
        ui->AltSetSelector->clear();
        m_endpoints.clear();
        ui->EndpointSelector->clear();
        updateSelector(ui->InterfaceSelector, m_interfaces);
   }
   

}

void USBDeviceImportEditor::setAltSet(QString interface){
    
    m_altSets.clear();
    if(m_interfaces.contains(interface) == true){

        m_interfaceNum = m_interfaces.indexOf(interface);
    
        libusb_config_descriptor *config;
        libusb_get_active_config_descriptor(m_dev, &config);
        
        const libusb_interface *inter = &config->interface[m_interfaceNum];
        const libusb_interface_descriptor *interdesc;

        for(int i = 0; i < (int)inter->num_altsetting; i++){
            interdesc = &inter->altsetting[i];
            int endpts = (int)interdesc->bNumEndpoints;
            QString altSet = "Alternate Setting " + QString::number(i) + ": Number of endpoints: " + QString::number(endpts);
            m_altSets.append(altSet);
        }
        m_endpoints.clear();
        ui->EndpointSelector->clear();
        updateSelector(ui->AltSetSelector, m_altSets);
    }
}

void USBDeviceImportEditor::setEndpoint(QString altSet){
    m_endpoints.clear();
    if(m_altSets.contains(altSet) == true){

        m_altSetNum = m_altSets.indexOf(altSet);
    
        libusb_config_descriptor *config;
        libusb_get_active_config_descriptor(m_dev, &config);
        
        const libusb_interface *inter = &config->interface[m_interfaceNum];
        const libusb_interface_descriptor *interdesc = &inter->altsetting[m_altSetNum];
        const libusb_endpoint_descriptor *epdesc;

        for(int i = 0; i < (int)interdesc->bNumEndpoints; i++){
            epdesc = &interdesc->endpoint[i];
            int endpts = (int)epdesc->bEndpointAddress;
            if(endpts >= 129){
            QString endpt = "Endpoint: " + QString::number(i) + ": Endpoint Address: " + QString::number(endpts);
            m_endpoints.append(endpt);
            }
        }
        updateSelector(ui->EndpointSelector, m_endpoints);
    }
}

void USBDeviceImportEditor::updateSelector(QComboBox *selector, QStringList items){
    selector->clear();
    selector->addItems(items);
}
