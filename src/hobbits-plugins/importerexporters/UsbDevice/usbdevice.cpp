#include "usbdevice.h"
#include "usbdeviceimporteditor.h"
#include <libusb-1.0/libusb.h>
#include <iostream>
#include <chrono>
#include <thread>


/**
 * @brief Construct a new USBDevice::USBDevice object and setup the parameters and parameter helper.
 * 
 */
USBDevice::USBDevice() 
{
    QList<ParameterDelegate::ParameterInfo> importInfos = {
        {"DeviceNum", ParameterDelegate::ParameterType::Integer},
        {"InterfaceNum", ParameterDelegate::ParameterType::Integer},
        {"AltSetNum", ParameterDelegate::ParameterType::Integer},
        {"EndpointNum", ParameterDelegate::ParameterType::Integer},
        {"TransferNum", ParameterDelegate::ParameterType::Integer},
        {"TransferDelay", ParameterDelegate::ParameterType::Integer},
        {"TransferTimeout", ParameterDelegate::ParameterType::Integer},
        {"TransferType", ParameterDelegate::ParameterType::Integer},
        {"TransferSize", ParameterDelegate::ParameterType::Integer},
        {"DeviceName", ParameterDelegate::ParameterType::String},
        {"TimeoutIn", ParameterDelegate::ParameterType::Boolean}
    };

    m_importDelegate = ParameterDelegate::create(
                    importInfos,
                    [this](const Parameters &parameters) {
                        return QString("%1 Import").arg(this->name());
                    },
                    [](QSharedPointer<ParameterDelegate> delegate, QSize size) {
                        Q_UNUSED(size)
                        return new USBDeviceImportEditor(delegate);
                    });

    QList<ParameterDelegate::ParameterInfo> exportInfos = {
    };

     m_exportDelegate = nullptr;
}

/**
 * @brief Creates a new USBDevice Importer Exporter Interface
 * 
 * @return ImporterExporterInterface* - the actual Importer that imports the data from USB
 */
ImporterExporterInterface* USBDevice::createDefaultImporterExporter()
{
    return new USBDevice();
}

/**
 * @brief Returns the name of the Plugin
 * 
 * @return QString - the name of the plugin
 */
QString USBDevice::name()
{
    return "USB Device";
}

/**
 * @brief Returns a description of the plugin
 * 
 * @return QString - a description of the plugin
 */
QString USBDevice::description()
{
    return "Imports the data sent Device->Host from a Selected USB Device Endpoint";
}

/**
 * @brief Returns the relevant tags for the plugin
 * 
 * @return QStringList - the relevant tags of the plugin
 */
QStringList USBDevice::tags()
{
    return {"Generic"};
}

/**
 * @brief Tells hobbits if the plugin can export data
 *  
 * @return false - plugin cannot export data so always returns false.
 */
bool USBDevice::canExport()
{
    return false;
}

/**
 * @brief Tells hobbits if plugin can import data depending on the OS being built for.
 * 
 * @return true - can only import data if not on Windows so returns true if not on windows
 * @return false - can only import data if not on Windows so returns false if on Windows.
 */
bool USBDevice::canImport()
{
    #ifdef _WIN32
    return false;
    #else
    return true;
    #endif
}

/**
 * @brief Returns the parameter delegate from the importer parameter delegate
 * 
 * @return QSharedPointer<ParameterDelegate> - the importer parameter delegate and parameters 
 */
QSharedPointer<ParameterDelegate>  USBDevice::importParameterDelegate()
{
    return m_importDelegate;
}

/**
 * @brief Returns the parameter delegate and parameters from the exporter parameter delegate, even though
 *  the exporter files don't exist DO NOT REMOVE as it could break plugin IDK why.
 * 
 * @return QSharedPointer<ParameterDelegate> - the non-existent parameter delegate and parameters from the non-existent 
 * exporter interface
 */
QSharedPointer<ParameterDelegate>  USBDevice::exportParameterDelegate()
{
    return m_exportDelegate;
}

/**
 * @brief initializes libusb session to m_ctx, gets the device, gets the config descriptor, gets the relevant information,
 *  gets the endpoint addr, and sets the device handle returns an int depending on success of the initialization.
 * 
 * @return int - returns 0 on success and returns a negative number on a failure or error, -1 on libusb init failure,
 * -2 on libusb device handle no mem failure, -3 on libusb handle no access failure, -4 on device handle no device failure.
 */
int USBDevice::setupLibusb(){
   int r = libusb_init(&m_ctx); //try initializing libusb
    if(r < 0){ //checking for errors, if r < 0  then there is an error
        return -1;
    }
   libusb_set_option(m_ctx, LIBUSB_OPTION_LOG_LEVEL, 2 ); //set terminal output for errors and warnings only
   libusb_get_device_list(m_ctx, &m_devs);
   m_dev = m_devs[m_deviceNum];
   libusb_free_device_list(m_devs, m_deviceNum);

   //getting the endpoint address
   libusb_get_active_config_descriptor(m_dev, &m_config);

   const libusb_interface *inter = &m_config->interface[m_interfaceNum];
   const libusb_interface_descriptor *interdesc = &inter->altsetting[m_altSetNum];
   const libusb_endpoint_descriptor *epdesc = &interdesc->endpoint[m_endpointNum];

   m_endpoint = epdesc->bEndpointAddress; 
   
   //try to open a libusb device handle
   r = libusb_open(m_dev, &m_handle); 
    if(r != 0){ //check for error
        switch (r) //return error codes if any
        {
        case LIBUSB_ERROR_NO_MEM:
            return -2;
            break;

        case LIBUSB_ERROR_ACCESS:
            return -3;
            break;
        
         case LIBUSB_ERROR_NO_DEVICE:
            return -4;
            break;
        default:
            return -5;
            break;
        }
    return 0;
    }
}

/**
 * @brief closes all the libusb variables and exits the libusb session after reading all needed data or encountering an error.
 * 
 * @param closeDevice  is there a device handle initialized to be able to close, not always true as if there
 *  is an error initializing a device handle. 
 */
void USBDevice::exitLibusb(bool closeDevice){
    if(closeDevice){ //check if there is a device handle to close
    libusb_close(m_handle);
    }
    libusb_free_config_descriptor(m_config);
    libusb_exit(m_ctx);
}

/**
 * @brief The function to actually read the binary data from the USB device, imports parameters, determines the transfer needed,
 * checks for device handle errors, detaches the kernel driver, reads the data, resets the device, updates the progressbar, 
 * checks if action is cancelled and then waits for the duration of transfer delay, and repeats for the amount in NumTransfers
 * 
 * @param parameters the parameters from the importer UI needed to set libusb to read the data form the proper device and endpoint
 * @param progress the pointer resposible for updating the progressbar and checking for cancelled tasks
 * @return QSharedPointer<ImportResult> - the pointer towards the bitcontainer so that hobbits can read the data being imported
 */
QSharedPointer<ImportResult> USBDevice::importBits(const Parameters &parameters,
                                                      QSharedPointer<PluginActionProgress> progress)
{
    //validating params
    QStringList invalidations = m_importDelegate->validate(parameters);
    if (!invalidations.isEmpty()) {
        return ImportResult::error(QString("Invalid parameters passed to %1:\n%2").arg(name()).arg(invalidations.join("\n")));
    }

    //getting parameters
    m_deviceNum = parameters.value("DeviceNum").toInt();
    m_interfaceNum = parameters.value("InterfaceNum").toInt();
    m_altSetNum = parameters.value("AltSetNum").toInt();
    m_endpointNum = parameters.value("EndpointNum").toInt();
    int transferNum = parameters.value("TransferNum").toInt();
    int transferDelay = parameters.value("TransferDelay").toInt();
    int transferTimeout = parameters.value("TransferTimeout").toInt();
    int transferType = parameters.value("TransferType").toInt();
    int transferSize = parameters.value("TransferSize").toInt();
    QString transferTypeStr = QString::number(transferType);
    QString deviceName = parameters.value("DeviceName").toString();
    bool includeTimeout = parameters.value("TimeoutIn").toBool();
    QSharedPointer<RangeSequence> frames = RangeSequence::createEmpty();
    QByteArray largeBuffer;
    int actualLength;
    unsigned char smallBuffer[1024];
    bool attach;
    int setupErr;


    //determine transfer type, as some transfers we can't handle yet.
    switch (transferType)
    {
    case 0:
        return ImportResult::error("Control Transfer Endpoints Not Supported");
        break;

    case 1: 
        return ImportResult::error("Isochronous Transfer Endpoints Not Supported");
        break;

    case 2: 
        transferTypeStr += ", Bulk Transfer"; //for setting metadata
        setupErr = setupLibusb(); //try to init libusb
        switch (setupErr) //raise errors if any are encountered
        {
        case -1:
            return ImportResult::error("Error Initializing Libusb, restart hobbits and try again.");
            break;
        
        case -2:
            exitLibusb(false);
            return ImportResult::error("Error Allocating Memory for Device, restart hobbits and try again.");
            break;
        
        case -3:
            exitLibusb(false);
            return ImportResult::error("Error Opening Device, no access to device, restart hobbits in root/Administrator and try again.");
            break;

        case -4:
            exitLibusb(false);
            return ImportResult::error("Error No Device Found, check device and try again.");
            break;

        case -5:
            exitLibusb(false);
            return ImportResult::error("Error Opening Device, Libusb ran into an error with its processing.");
            break;

        default:
            break;
        }
        for(int i = 0; i < transferNum; i++){ //for every transfer
            if(libusb_kernel_driver_active(m_handle, m_interfaceNum) == 1){ //check if the kernel has a driver active
                attach = true; //need to reattach driver later
                libusb_detach_kernel_driver(m_handle, m_interfaceNum); //dettach driver now 
            }
            //transfer time
            int transferReturn = libusb_bulk_transfer(m_handle, m_endpoint, smallBuffer,(int)sizeof(smallBuffer), &actualLength, transferTimeout);
            if(transferReturn == 0 && actualLength != 0){ //check that the transfer is valid
                frames->appendRange(transferSize*8); //create a new frame on every transfer
                for(int j = 0; j < transferSize; j++){
                    largeBuffer.append(smallBuffer[j]); //add the data from the data buffer to the bigger resizable buffer
                }   
            }else{
                    if(transferReturn != LIBUSB_ERROR_TIMEOUT){// check if an error was raised that isnt a timeout
                        if(attach){
                            libusb_reset_device(m_handle); //reset the device reattaching the kernel driver
                            attach = false;
                        }
                        //throw an error if an error occured
                        return ImportResult::error("Transfer Error, Code: " + QString::number(transferReturn) + " " + libusb_error_name(transferReturn) + ": " + libusb_strerror(transferReturn));
                    }else{
                        if(includeTimeout){ //check if the user wants to include timeout
                            largeBuffer.append("TIMEOUT"); 
                            frames->appendRange(56);
                        }
                    }
            }
            if(attach){
                libusb_reset_device(m_handle); //reset the device to reattach the kernel driver
                attach = false;
            }
            if(progress->isCancelled()){ // check if the user has cancelled the import operation
                break;
            }
            progress->setProgress(i, transferNum);  //update the progressbar
            std::this_thread::sleep_for(std::chrono::milliseconds(transferDelay)); //wait for the durration of the transfer delay

        }
        exitLibusb(true); //exit libusb with closing the device
        break;
    case 3:
        /**
         * This is the same thing as the bulk transfer implementation, but instead with an interrup transfer, only 2 lines of
         * code are different, 244/322 and 281/358 refer to the bulk transfer implementation for a thorough explanation.
         * 
         */

       transferTypeStr += ", Interrupt Transfer";
        setupErr = setupLibusb();
        switch (setupErr)
        {
        case -1:
            return ImportResult::error("Error Initializing Libusb, restart hobbits and try again.");
            break;
        
        case -2:
            exitLibusb(false);
            return ImportResult::error("Error Allocating Memory for Libusb, restart hobbits and try again.");
            break;
        
        case -3:
            exitLibusb(false);
            return ImportResult::error("Error Opening Device, no access to device, restart hobbits in root/Administrator and try again.");
            break;

        case -4:
            exitLibusb(false);
            return ImportResult::error("Error No Device Found, check device and try again.");
            break;

        case -5:
            exitLibusb(false);
            return ImportResult::error("Error Opening Device, Libusb ran into an error with its processing.");
            break;
    
        default:
            break;
        }
        for(int i = 0; i < transferNum; i++){
            if(libusb_kernel_driver_active(m_handle, m_interfaceNum) == 1){
                attach = true;
                libusb_detach_kernel_driver(m_handle, m_interfaceNum);
            }
            int transferReturn = libusb_interrupt_transfer(m_handle, m_endpoint, smallBuffer,(int)sizeof(smallBuffer), &actualLength, transferTimeout);
            if(transferReturn == 0 && actualLength != 0){
                frames->appendRange(transferSize*8);
                for(int j = 0; j < transferSize; j++){
                    largeBuffer.append(smallBuffer[j]);
                }   
            }else{
                if(transferReturn != LIBUSB_ERROR_TIMEOUT){
                    if(attach){
                        libusb_reset_device(m_handle);
                        attach = false;
                    }
                    return ImportResult::error("Transfer Error, Code: " + QString::number(transferReturn) + " " + libusb_error_name(transferReturn) + ": " + libusb_strerror(transferReturn));
                }else{
                    if(includeTimeout){
                        largeBuffer.append("TIMEOUT");
                        frames->appendRange(56);
                    }
                }
            }
            if(attach){
                libusb_reset_device(m_handle);
                attach = false;
            }
            if(progress->isCancelled()){
                break;
            }
            progress->setProgress(i, transferNum);  
            std::this_thread::sleep_for(std::chrono::milliseconds(transferDelay));

        }
        exitLibusb(true);
        break;
    
    default:
        break;
    }
    QSharedPointer<BitContainer> container = BitContainer::create(largeBuffer); //creating a bit container with all the data in it
    container->setName("USB " + deviceName); //esetting the name you see on the side of hobbits
    QSharedPointer<BitInfo> info = BitInfo::create(container->bits()->sizeInBits()); //creating a bit infor for setting frames
    info->setFrames(frames); // setting frames on every transfer

    //setting the metadata
    info->setMetadata("Device Name", deviceName);
    info->setMetadata("Device Number", m_deviceNum);
    info->setMetadata("Interface Number", m_interfaceNum);
    info->setMetadata("Alternate Setting Number", m_altSetNum);
    info->setMetadata("Endpoint Number", m_endpointNum);
    info->setMetadata("Endpoint Address", (int)m_endpoint);
    info->setMetadata("Number of Transfers", transferNum);
    info->setMetadata("Transfer Delay Duration", transferDelay);
    info->setMetadata("Transfer Timeout Duration", transferTimeout);
    info->setMetadata("Transfer Type", transferTypeStr);
    info->setMetadata("TransferSize", transferSize);

    //adding the info to the BitContainer
    container->setInfo(info);
   
    //returning the bit container
    return ImportResult::result(container, parameters);
}

/**
 * @brief The function that would export the bits if the plugin allowed for exporting bits, DO NOT REMOVE
 * it may break hobbits without it IDK why.
 * 
 * @param container The container to export
 * @param parameters the parameters require for the export
 * @param progress the progressbar and progress checker for the output
 * @return QSharedPointer<ExportResult> - a pointer towards the export result, right now throws an error if its ever called 
 */
QSharedPointer<ExportResult> USBDevice::exportBits(QSharedPointer<const BitContainer> container,
                                                      const Parameters &parameters,
                                                      QSharedPointer<PluginActionProgress> progress)
{
    return ExportResult::error("Export not implemented");
}
