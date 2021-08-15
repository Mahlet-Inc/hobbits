#include "usbdeviceimporteditor.h"
#include "ui_usbdeviceimporteditor.h"
#include <libusb-1.0/libusb.h>
#include <string>
#include <iostream>
#include <sstream>


/**
 * @brief Construct a new UsbDeviceImportEditor::UsbDeviceImportEditor object, sets ui elements, initializes libusb gets a device
 * list, populates the device selector ComboBox and sets Signals and Slots.
 * 
 * @param delegate a pointer that points to a ParameterDelegate object, used to initialize the parameter helper object,
 * to transfer UI parameters to the importBits function in usbdevice.cpp.
 * 
 */

UsbDeviceImportEditor::UsbDeviceImportEditor(QSharedPointer<ParameterDelegate> delegate):
    ui(new Ui::UsbDeviceImportEditor()),
    m_paramHelper(new ParameterHelper(delegate))
  
{

    //QT stuff to setup the UI elements and class
    ui->setupUi(this);

    
    //setting our libusb context to be able to get the device list and information
    initLibusb();

    //creating parameters for the various UI elements and their needed data
    m_paramHelper->addComboBoxParameter("DeviceNum", ui->DeviceSelector, Qt::UserRole);
    m_paramHelper->addComboBoxParameter("InterfaceNum", ui->InterfaceSelector, Qt::UserRole);
    m_paramHelper->addComboBoxParameter("AltSetNum", ui->AltSetSelector, Qt::UserRole);
    m_paramHelper->addComboBoxParameter("EndpointNum", ui->EndpointSelector, Qt::UserRole);
    m_paramHelper->addSpinBoxIntParameter("TransferNum", ui->TransferNum);
    m_paramHelper->addSpinBoxIntParameter("TransferDelay", ui->TransferDelay);
    m_paramHelper->addSpinBoxIntParameter("TransferTimeout", ui->TransferTimeout);
    m_paramHelper->addCheckBoxBoolParameter("TimeoutIn", ui->IncludeTimeoutCheck);

    //getting a list of USB devices, and their information
    m_devices = this->getUsbDevices();

    //populating the device selector with the list of devices
    this->updateSelector(ui->DeviceSelector, m_devices);

    //setting signals and slots for each combobox/selector on the UI
    connect(ui->DeviceSelector, SIGNAL(currentTextChanged(const QString &)), this, SLOT(populateInterfaces(const QString &)));
    connect(ui->InterfaceSelector, SIGNAL(currentTextChanged(const QString &)), this, SLOT(populateAltSet(const QString &)));
    connect(ui->AltSetSelector, SIGNAL(currentTextChanged(const QString &)), this, SLOT(populateEndpoint(const QString &)));
    connect(ui->EndpointSelector, SIGNAL(currentTextChanged(const QString &)), this, SLOT(configureEndpoint(const QString &)));

}

/**
 * @brief Destroy the UsbDeviceImportEditor::UsbDeviceImportEditor object, free libusb variables, and delete the ui.
 * 
 * Frees the device list, by deleting the libusb_device pointer, exits libusb, and deletes the ui class, then deconstructs the 
 * UsbDeviceImportEditor object.
 * 
 */
UsbDeviceImportEditor::~UsbDeviceImportEditor()
{
    libusb_free_device_list(m_devs, m_deviceNum); 
    libusb_exit(m_ctx);
    delete ui;
}

/**
 * @brief Returns the title to be used on the UI window.
 * 
 * @return QString the title to be displayed on the top of the UI window 
 */
QString UsbDeviceImportEditor::title()
{
    return "Select USB Device & Read Configuurations";
}

/**
 * @brief get all the necessary parameters from the program and the UI and return them to the main parameter delegate.
 * 
 * Gets parameters from the selectors and the spin boxes, as well the transfer type, size, and device name, determined
 * by functions below.
 * 
 * @return Parameters - the various parameters from the users UI selection, and other parameters determined by the program. 
 */
Parameters UsbDeviceImportEditor::parameters()
{
    auto params = m_paramHelper->getParametersFromUi(); //gather our parameter values from the ui
    params.insert("TransferType", (int)m_transferType); // adding the transfer type parameter
    params.insert("TransferSize", (int)m_transferSize); // adding the transfer size parameter
    params.insert("DeviceName", m_device);
    return params;
}

/**
 * @brief apply the parameters provided to the UI elements.
 * 
 * Applies any parameters values provided through the function parameter, to the relevant UI
 * 
 * @param parameters parameters and their UI elements to apply to 
 * @return bool - returns true if successful, false if unsucessful
 * 
 */
bool UsbDeviceImportEditor::setParameters(const Parameters &parameters)
{
    return m_paramHelper->applyParametersToUi(parameters);
}

/**
 * @brief initialize the libusb session and get the device list so we can populate the UI.
 * 
 * Initializes a libusb session and gets a device list so that we can populate the device, interface,
 * alt set, and endpoint selectors. Throws errors if libusb raises an error in initalization or getting the device list.
 * 
 */
void UsbDeviceImportEditor::initLibusb(){
    m_devices.clear(); //clearing the device list
    int r = libusb_init(&m_ctx); //initialize a library session
	if(r < 0) { //check if error, if there is an error r will be less than 0
        std::runtime_error libusb_init_error("Error while trying to initialize Libusb");
		throw libusb_init_error;
	}
    m_cnt = libusb_get_device_list(m_ctx, &m_devs);
    if (m_cnt < 0){ //check if there is an error, if there is an error m_cnt will be less than 0
        std::runtime_error libusb_device_list_error("Error getting device list");
        throw libusb_device_list_error;
    }
}

/**
 * @brief Get a list of strings describing devices to populate the device selector.
 * 
 * Creates and returns a list of strings describing the various USB devices recognized by libusb, 
 * the string contains the device number, the vendor and product ID, and the vendor and product names.
 * 
 * @return QStringList - list of strings describing devices recognized by libusb, used to populate the device selector 
 */  
QStringList UsbDeviceImportEditor::getUsbDevices(){

    QStringList devices; //the QStringList we will return
    for(int i = 0; i < m_cnt; i++){ //for every device in the libusb device list
        libusb_device *dev = m_devs[i]; //get the device
        libusb_device_descriptor desc; 
        libusb_get_device_descriptor(dev, &desc); //get the device descriptor

        QString idVendor = QString("%1").arg((int)desc.idVendor, 4, 16, QChar('0'));
        QString idProduct = QString("%1").arg((int)desc.idProduct, 4, 16, QChar('0'));

        QStringList productAndVendor = getVendorAndProduct(idVendor, idProduct); //getting the vendor and product names

        QString temp = "Device " + QString::number(i) + ": Vendor ID: " + idVendor + ", Product ID: " + idProduct + " " + productAndVendor[0] + " " + productAndVendor[1];
        devices.append(temp); // appending the device desciption string to the QStringList
    }
    return devices;

}

/**
 * @brief Populates the interface selector/ComboBox with the various interfaces of a device that was selected from the 
 * device selector. Also clears the AltSetSelector and EndpointSelector ComboBoxes if it was already populated.
 * Function should only be used as a slot for a signal. 
 * 
 * @param device The QString that was selected from the DeviceSelector ComboBox
 */
void UsbDeviceImportEditor::populateInterfaces(QString device){
    
    
    m_device = device; // setting the global device descriptor string
   m_interfaces.clear(); //clear the interfaces selector
   if(m_devices.contains(device) == true){ //check if the device selected is actually in the devices list

        m_deviceNum = m_devices.indexOf(device); 
        m_dev = m_devs[m_deviceNum]; //set the libusb device
        libusb_config_descriptor *config;
        libusb_get_active_config_descriptor(m_dev, &config); //get the descriptor required to get info 
        //about the interfaces of the device

        const libusb_interface *inter;

        for(int i  = 0; i < (int)config->bNumInterfaces; i++){ // for every interface on the device
            inter = &config->interface[i];
            int altSets = inter->num_altsetting; //find the number of alt sets
            //make an interface output string
            QString interface = "Interface " + QString::number(i) + ": Number of Alternate Settings: " + QString::number(altSets);
            m_interfaces.append(interface);
        }

        //clear the below selectors
        m_altSets.clear();
        ui->AltSetSelector->clear();
        m_endpoints.clear();
        ui->EndpointSelector->clear();
        
        //free data and update selectors
        libusb_free_config_descriptor(config);
        updateSelector(ui->InterfaceSelector, m_interfaces);
   }
   

}
/**
 * @brief Populates the AltSetSelector ComboBox with the various alternate settings of an interface that was selected from the 
 * interface selector. Also clears the EndpointSelector ComboBox if it was already populated. 
 * Function should only be used as a slot for a signal. 
 * 
 * @param interface The QString describing the interface selected from the InterfaceSelector ComboBox
 */
void UsbDeviceImportEditor::populateAltSet(QString interface){
    
    m_altSets.clear();
    if(m_interfaces.contains(interface) == true){

        m_interfaceNum = m_interfaces.indexOf(interface);
    
        libusb_config_descriptor *config;
        libusb_get_active_config_descriptor(m_dev, &config);
        
        const libusb_interface *inter = &config->interface[m_interfaceNum];
        const libusb_interface_descriptor *interdesc; //used to choose alt set

        for(int i = 0; i < (int)inter->num_altsetting; i++){ //for every alternate setting in the interface
            interdesc = &inter->altsetting[i];
            int endpts = (int)interdesc->bNumEndpoints; //get the number of endpoints in the current alt set
            //create the alt set output string
            QString altSet = "Alternate Setting " + QString::number(i) + ": Number of endpoints: " + QString::number(endpts);
            m_altSets.append(altSet);
        }
        //clear the endpoint selector
        m_endpoints.clear();
        ui->EndpointSelector->clear();
        
        libusb_free_config_descriptor(config);
        updateSelector(ui->AltSetSelector, m_altSets);
    }
}

/**
 * @brief Populates the EndpointSelector ComboBox with the various Endpoints of an Alternate Setting that was selected from the 
 * Alternate Setting selector. Checks if endpoint is an IN or OUT endpoint and only populates with IN endpoints. 
 * Function should only be used as a slot for a signal. 
 * 
 * @param altSet The QString describing the Alternate Setting selected from the AltSetSelector ComboBox
 */
void UsbDeviceImportEditor::populateEndpoint(QString altSet){
    m_endpoints.clear();
    if(m_altSets.contains(altSet) == true){

        m_altSetNum = m_altSets.indexOf(altSet);
    
        libusb_config_descriptor *config;
        libusb_get_active_config_descriptor(m_dev, &config);
        
        const libusb_interface *inter = &config->interface[m_interfaceNum];
        const libusb_interface_descriptor *interdesc = &inter->altsetting[m_altSetNum];
        const libusb_endpoint_descriptor *epdesc; //used to choose endpt

        for(int i = 0; i < (int)interdesc->bNumEndpoints; i++){ //for every endpoint in the alt set
            epdesc = &interdesc->endpoint[i];
            int endpts = (int)epdesc->bEndpointAddress; //get the endpoint addr
            if(endpts >= 129){ //check if the endpoint is an IN endpoint, if endpoint addr is greater than or equal to 129
                               //then the endpoint is an IN EP
            QString endpt = "Endpoint: " + QString::number(i) + ": Endpoint Address: " + QString::number(endpts);
            m_endpoints.append(endpt);
            }
        }
        libusb_free_config_descriptor(config);
        updateSelector(ui->EndpointSelector, m_endpoints);
    }
}

/**
 * @brief Gets the necessary information about the endpoint selected from the EndpointSelector ComboBox,
 * like the transfer type, and the packet size to read. Function should only be used as a Slot to a Signal.
 * 
 * @param endpoint The QString of the endpoint selected from the EndpointSelector ComboBox 
 */
void UsbDeviceImportEditor::configureEndpoint(QString endpoint){
    if(m_endpoints.contains(endpoint)){
        m_endpointNum = m_endpoints.indexOf(endpoint);

        libusb_config_descriptor *config;
        libusb_get_active_config_descriptor(m_dev, &config);
        
        const libusb_interface *inter = &config->interface[m_interfaceNum];
        const libusb_interface_descriptor *interdesc = &inter->altsetting[m_altSetNum];
        const libusb_endpoint_descriptor *epdesc = &interdesc->endpoint[m_endpointNum]; //used to get the endpoint data

        m_transferType = epdesc->bmAttributes; //bmAttributes is an 8 bit number reresenting the transfer type:
                                               // 0 for control, 1 for isochronous, 2 for bulk, 3 for interrupt
        m_transferSize = epdesc->wMaxPacketSize; //get the maximum number of bytes in a transfer
        libusb_free_config_descriptor(config);
    }
}

/**
 * @brief Updates the QComboBox passed as the selector parameter with the QStringList of items to populate the ComboBox with.
 * 
 * @param selector The QComboBox to clear and populate.
 * @param items The QStringList of items to populate the selector with.
 */
void UsbDeviceImportEditor::updateSelector(QComboBox *selector, QStringList items){
    selector->clear();
    for(int i = 0; i < items.length(); i++){
        selector->addItem(items[i], i);
    }
}

/**
 * @brief Adds the leading zeros to the hex string passed into the parameter str, and returns the new string
 * useful for getting the list of devices, and identifiying their vendor and product name from ID.
 * 
 * @param str std::string, the hex string of either the vendorID or productID
 * @return QString - the hex string, but with the proper number of leading zeroes added to it. 
 */
QString UsbDeviceImportEditor::addLeadingZeros(std::string str){
    switch (str.length())
    {
    case 1: 
        return "000" + QString::fromStdString(str);
        break;
    case 2:
        return "00" + QString::fromStdString(str);
        break;
    case 3:
        return "0" + QString::fromStdString(str);
        break;
    default:
        return QString::fromStdString(str);
        break;
    }
}

/**
 * @brief Gets the name of the Vendor and the Product in a human understandable string, based of the product and vendor IDs 
 * passed as parameters
 * 
 * @param idVendor the Hex String of the Vendor ID
 * @param idProduct the Hex String of the Product ID
 * @return QStringList - a list of 2 QStrings the First being the Vendor name, the second being the Product name or
 *  "Product Name not Found" if the product name is not in the list.
 */
QStringList UsbDeviceImportEditor::getVendorAndProduct(QString idVendor, QString idProduct){
    bool findProduct = false; //true if the vendor has been identified
    QStringList returnVals; //the values to return
    QFile vendorProductFile(":/usbresources/usb_resources/usb.ids"); //the resource file that needs to be opened has been complied into the MOC 
                                                                    //so no need to worry about file location
    if(!vendorProductFile.open(QIODevice::ReadOnly)){ //open the file to read in read only mode
        std::runtime_error file_read_error("Error opening USB Vendor and Product ID file");
        throw file_read_error;
    }

    QTextStream in(&vendorProductFile); //each line of the file
    while (!in.atEnd()) { //mainloop terminates at the end of file or on break
        QString line = in.readLine(); //reads the next line into memory
        if(line.startsWith("#")){ //lines starting with # are comment lines and can be disregarded
            continue;
        }
        else if(!line.startsWith("\t")){ //are we on a vendor line, a \t signifies a product line 
            if(findProduct){ // are we looking for a product? if so then we have gone too far and the product name does not exist
                returnVals.append("Product Name Not Found");
                return returnVals; //return vendor and "product name not found"
            }
            else
            {
                if(line.startsWith(idVendor)){ //if the line starts with the vendorID we are looking for
                    QStringList temp = line.split("  "); //there are two spaces that sepparate the vendorID and the vendor name
                    returnVals.append(temp[1]);
                    findProduct = true; //we have found the vendor now looking for the product
                }
            }
        }
        else if(line.startsWith("\t")){ //are we on a product line
            if(findProduct){ //if we are looking for a product
                line = line.remove("\t"); //then remove the tab
                if(line.startsWith(idProduct)){ //check for product id
                    QStringList temp = line.split("  "); //product id and name sepparated by two spaces
                    returnVals.append(temp[1]); //if found append the product name
                    return returnVals; //return the vendor and product name
                }
            }
            else
            {
                continue;
            }
        }
    }
    return returnVals;
}
