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

    
    initLibusb();

    m_paramHelper->addComboBoxParameter("DeviceNum", ui->DeviceSelector, Qt::UserRole);
    m_paramHelper->addComboBoxParameter("InterfaceNum", ui->InterfaceSelector, Qt::UserRole);
    m_paramHelper->addComboBoxParameter("AltSetNum", ui->AltSetSelector, Qt::UserRole);
    m_paramHelper->addComboBoxParameter("EndpointNum", ui->EndpointSelector, Qt::UserRole);
    m_paramHelper->addSpinBoxIntParameter("TransferNum", ui->TransferNum);
    m_paramHelper->addSpinBoxIntParameter("TransferDelay", ui->TransferDelay);
    m_paramHelper->addSpinBoxIntParameter("TransferTimeout", ui->TransferTimeout);

    m_devices = this->getUsbDevices();
    this->updateSelector(ui->DeviceSelector, m_devices);
    connect(ui->DeviceSelector, SIGNAL(currentTextChanged(const QString &)), this, SLOT(populateInterfaces(const QString &)));
    connect(ui->InterfaceSelector, SIGNAL(currentTextChanged(const QString &)), this, SLOT(populateAltSet(const QString &)));
    connect(ui->AltSetSelector, SIGNAL(currentTextChanged(const QString &)), this, SLOT(populateEndpoint(const QString &)));
    connect(ui->EndpointSelector, SIGNAL(currentTextChanged(const QString &)), this, SLOT(configureEndpoint(const QString &)));

}

USBDeviceImportEditor::~USBDeviceImportEditor()
{
    libusb_exit(m_ctx);
    delete ui;
}

QString USBDeviceImportEditor::title()
{
    return "Select USB Device & Configurations to Read From";
}

Parameters USBDeviceImportEditor::parameters()
{
    auto params = m_paramHelper->getParametersFromUi();
    params.insert("TransferType", (int)m_transferType);
    params.insert("TransferSize", (int)m_transferSize);
    params.insert("DeviceName", m_device);
    return params;
}

bool USBDeviceImportEditor::setParameters(const Parameters &parameters)
{
    return m_paramHelper->applyParametersToUi(parameters);
}


void USBDeviceImportEditor::initLibusb(){
    m_devices.clear();
    int r = libusb_init(&m_ctx); //initialize a library session
	if(r < 0) {
        std::runtime_error libusb_init_error("Error while trying to initialize Libusb");
		throw libusb_init_error;
	}
    libusb_set_option(m_ctx, LIBUSB_OPTION_LOG_LEVEL, 4 );
    m_cnt = libusb_get_device_list(m_ctx, &m_devs);
    if (m_cnt < 0){
        std::runtime_error libusb_device_list_error("Error getting device list");
        throw libusb_device_list_error;
    }
}
    
QStringList USBDeviceImportEditor::getUsbDevices(){

    //windows compatible, but not verbose enough so i would like to make it more verbose
    QStringList devices;
    for(int i = 0; i < m_cnt; i++){
        libusb_device *dev = m_devs[i];
        libusb_device_descriptor desc;
        libusb_get_device_descriptor(dev, &desc);
        std::stringstream sstream;
        sstream << std::hex << desc.idVendor;
        std::string idVendor = sstream.str();
        std::stringstream sstream2;
        sstream2 << std::hex << desc.idProduct;
        std::string idProduct = sstream2.str();
        QString temp = "Device " + QString::number(i) + ": Vendor ID: " + QString::fromStdString(idVendor) + ", Product ID: " + QString::fromStdString(idProduct);
        devices.append(temp);
    }
    return devices;

    //TODO: see if you can use the library/system that lsusb uses to get device information
    // auto obuf = sp::check_output({"lsusb"});
    // QString output = obuf.buf.data();
    // QStringList devices = output.split("\n");
    // devices.removeLast();
    // return devices;
}


//Redefine to populateInterfaces to make clearer
void USBDeviceImportEditor::populateInterfaces(QString device){
    
    
    m_device = device;
   m_interfaces.clear();
   if(m_devices.contains(device) == true){

        m_deviceNum = m_devices.indexOf(device);
        m_dev = m_devs[m_deviceNum];
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
        libusb_free_device_list(m_devs, m_deviceNum);
        libusb_free_config_descriptor(config);
        updateSelector(ui->InterfaceSelector, m_interfaces);
   }
   

}

void USBDeviceImportEditor::populateAltSet(QString interface){
    
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
        libusb_free_config_descriptor(config);
        updateSelector(ui->AltSetSelector, m_altSets);
    }
}

void USBDeviceImportEditor::populateEndpoint(QString altSet){
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
        libusb_free_config_descriptor(config);
        updateSelector(ui->EndpointSelector, m_endpoints);
    }
}
//rename to make clearer
void USBDeviceImportEditor::configureEndpoint(QString endpoint){
    if(m_endpoints.contains(endpoint)){
        m_endpointNum = m_endpoints.indexOf(endpoint);

        libusb_config_descriptor *config;
        libusb_get_active_config_descriptor(m_dev, &config);
        
        const libusb_interface *inter = &config->interface[m_interfaceNum];
        const libusb_interface_descriptor *interdesc = &inter->altsetting[m_altSetNum];
        const libusb_endpoint_descriptor *epdesc = &interdesc->endpoint[m_endpointNum];

        m_transferType = epdesc->bmAttributes;
        m_transferSize = epdesc->wMaxPacketSize;
        libusb_free_config_descriptor(config);
    }
}

void USBDeviceImportEditor::updateSelector(QComboBox *selector, QStringList items){
    selector->clear();
    for(int i = 0; i < items.length(); i++){
    selector->addItem(items[i], i);
    }
}


