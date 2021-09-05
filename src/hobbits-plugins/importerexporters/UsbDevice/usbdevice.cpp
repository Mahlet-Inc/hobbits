#include "usbdevice.h"
#include "usbdeviceimporteditor.h"
#include <libusb-1.0/libusb.h>
#include <iostream>
#include <chrono>
#include <thread>

/**
 * @brief Construct a new UsbDevice::UsbDevice object and setup the parameters and parameter helper.
 * 
 */
UsbDevice::UsbDevice()
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
        {"TimeoutIn", ParameterDelegate::ParameterType::Boolean}};

    m_importDelegate = ParameterDelegate::create(
        importInfos,
        [this](const Parameters &parameters)
        {
            QString dev = parameters.value("DeviceName").toString();
            int trNum = parameters.value("TransferNum").toInt();
            QString output = ", Import " + QString::number(trNum) + " Transfers From USB Device " + dev;
            return QString("%1" + output).arg(this->name());
        },
        [](QSharedPointer<ParameterDelegate> delegate, QSize size)
        {
            Q_UNUSED(size)
            return new UsbDeviceImportEditor(delegate);
        });

    QList<ParameterDelegate::ParameterInfo> exportInfos = {};

    m_exportDelegate = nullptr;
}

/**
 * @brief Creates a new UsbDevice Importer Exporter Interface
 * 
 * @return ImporterExporterInterface* - the actual Importer that imports the data from USB
 */
ImporterExporterInterface *UsbDevice::createDefaultImporterExporter()
{
    return new UsbDevice();
}

/**
 * @brief Returns the name of the Plugin
 * 
 * @return QString - the name of the plugin
 */
QString UsbDevice::name()
{
    return "USB Device";
}

/**
 * @brief Returns a description of the plugin
 * 
 * @return QString - a description of the plugin
 */
QString UsbDevice::description()
{
    return "Imports the data sent Device->Host from a Selected USB Device Endpoint";
}

/**
 * @brief Returns the relevant tags for the plugin
 * 
 * @return QStringList - the relevant tags of the plugin
 */
QStringList UsbDevice::tags()
{
    return {"Generic"};
}

/**
 * @brief Tells hobbits if the plugin can export data
 *  
 * @return false - plugin cannot export data so always returns false.
 */
bool UsbDevice::canExport()
{
    return false;
}

/**
 * @brief Tells hobbits if plugin can import data depending on the OS being built for.
 * 
 * @return true - can only import data if not on Windows so returns true if not on windows
 * @return false - can only import data if not on Windows so returns false if on Windows.
 */
bool UsbDevice::canImport()
{
    return true;
}

/**
 * @brief Returns the parameter delegate from the importer parameter delegate
 * 
 * @return QSharedPointer<ParameterDelegate> - the importer parameter delegate and parameters 
 */
QSharedPointer<ParameterDelegate> UsbDevice::importParameterDelegate()
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
QSharedPointer<ParameterDelegate> UsbDevice::exportParameterDelegate()
{
    return m_exportDelegate;
}

/**
 * @brief initializes libusb session to params.ctx, gets the device, gets the config descriptor, gets the relevant information,
 *  gets the endpoint addr, and sets the device handle returns an int depending on success of the initialization.
 * 
 * @param params the UsbParams struct that contains all the necessary data for libusb to read from the device properly
 * 
 * @return int - returns 0 on success and returns a negative number on a failure or error, -1 on libusb init failure,
 * -2 on libusb device handle no mem failure, -3 on libusb handle no access failure, -4 on device handle no device failure.
 */

//create parameters structure to handle variables
void UsbDevice::setupLibusb(UsbParams &params)
{
    int r = libusb_init(&params.ctx); //try initializing libusb
    if (r < 0)
    { //checking for errors, if r < 0  then there is an error
        params.errorCode = r;
    }
    libusb_get_device_list(params.ctx, &params.devs);
    params.dev = params.devs[params.deviceNum];
    libusb_free_device_list(params.devs, params.deviceNum);

    //getting the endpoint address
    libusb_get_active_config_descriptor(params.dev, &params.config);

    const libusb_interface *inter = &params.config->interface[params.interfaceNum];
    const libusb_interface_descriptor *interdesc = &inter->altsetting[params.altSetNum];
    const libusb_endpoint_descriptor *epdesc = &interdesc->endpoint[params.endpointNum];

    params.endpoint = epdesc->bEndpointAddress;

    //try to open a libusb device handle
    r = libusb_open(params.dev, &params.handle);

    params.errorCode = r;
}

/**
 * @brief closes all the libusb variables and exits the libusb session after reading all needed data or encountering an error.
 * 
 * @param closeDevice  is there a device handle initialized to be able to close, not always true as if there
 *  is an error initializing a device handle. 
 * @param params a UsbParams struct that handles all the parameters that need to be passed for libusb to close properly
 */
void UsbDevice::exitLibusb(bool closeDevice, UsbParams &params)
{
    if (closeDevice)
    { //check if there is a device handle to close
        libusb_close(params.handle);
    }
    libusb_free_config_descriptor(params.config);
    libusb_exit(params.ctx);
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
QSharedPointer<ImportResult> UsbDevice::importBits(const Parameters &parameters,
                                                   QSharedPointer<PluginActionProgress> progress)
{
    //validating params
    QStringList invalidations = m_importDelegate->validate(parameters);
    if (!invalidations.isEmpty())
    {
        return ImportResult::error(QString("Invalid parameters passed to %1:\n%2").arg(name()).arg(invalidations.join("\n")));
    }

    //setting up our struct
    UsbParams params;
    //getting parameters
    params.deviceNum = parameters.value("DeviceNum").toInt();
    params.interfaceNum = parameters.value("InterfaceNum").toInt();
    params.altSetNum = parameters.value("AltSetNum").toInt();
    params.endpointNum = parameters.value("EndpointNum").toInt();
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

    //determine transfer type, as some transfers we can't handle yet.
    
    if (transferType == 0)
    {
        return ImportResult::error("Control Transfer Endpoints Not Supported");
    }
    else if(transferType == 1)
    {
        return ImportResult::error("Isochronous Transfer Endpoints Not Supported");
    }
    else if(transferType == 2)
    {
        transferTypeStr += ", Bulk Transfer"; //for setting metadata
        setupLibusb(params);             //try to init libusb
        if (params.errorCode < 0)
        {
            return returnError(params.errorCode);
        }
        for (int i = 0; i < transferNum; i++)
        { //for every transfer
            if (libusb_kernel_driver_active(params.handle, params.interfaceNum) == 1)
            {                                                          //check if the kernel has a driver active
                attach = true;                                         //need to reattach driver later
                libusb_detach_kernel_driver(params.handle, params.interfaceNum); //dettach driver now
            }
            //transfer time
            int transferReturn = libusb_bulk_transfer(params.handle, params.endpoint, smallBuffer, (int)sizeof(smallBuffer), &actualLength, transferTimeout);
            if (transferReturn == 0 && actualLength != 0)
            {                                          //check that the transfer is valid
                frames->appendRange(transferSize * 8); //create a new frame on every transfer
                for (int j = 0; j < transferSize; j++)
                {
                    largeBuffer.append(smallBuffer[j]); //add the data from the data buffer to the bigger resizable buffer
                }
            }
            else
            {
                if (transferReturn != LIBUSB_ERROR_TIMEOUT)
                { // check if an error was raised that isnt a timeout
                    if (attach)
                    {
                        libusb_reset_device(params.handle); //reset the device reattaching the kernel driver
                        attach = false;
                    }
                    //throw an error if an error occured
                    return returnError(transferReturn);
                }
                else
                {
                    if (includeTimeout)
                    { //check if the user wants to include timeout
                        largeBuffer.append("TIMEOUT");
                        frames->appendRange(56);
                    }
                }
            }
            if (attach)
            {
                libusb_reset_device(params.handle); //reset the device to reattach the kernel driver
                attach = false;
            }
            if (progress->isCancelled())
            { // check if the user has cancelled the import operation
                break;
            }
            progress->setProgress(i, transferNum);                                 //update the progressbar
            std::this_thread::sleep_for(std::chrono::milliseconds(transferDelay)); //wait for the durration of the transfer delay
        }
        exitLibusb(true, params); //exit libusb with closing the device
    }
    else if (transferType == 3)
    {
        /**
         * This is the same thing as the bulk transfer implementation, but instead with an interrup transfer, only 2 lines of
         * code are different, these lines replace bulk transfer with interrupt transfer.
         * Refer to the bulk transfer implementation for a thorough explanation.
         * 
         */

        transferTypeStr += ", Interrupt Transfer";
        setupLibusb(params);

        if (params.errorCode < 0)
        {
            return returnError(params.errorCode);
        }

        for (int i = 0; i < transferNum; i++)
        {
            if (libusb_kernel_driver_active(params.handle, params.interfaceNum) == 1)
            {
                attach = true;
                libusb_detach_kernel_driver(params.handle, params.interfaceNum);
            }
            int transferReturn = libusb_interrupt_transfer(params.handle, params.endpoint, smallBuffer, (int)sizeof(smallBuffer), &actualLength, transferTimeout);
            if (transferReturn == 0 && actualLength != 0)
            {
                frames->appendRange(transferSize * 8);
                for (int j = 0; j < transferSize; j++)
                {
                    largeBuffer.append(smallBuffer[j]);
                }
            }
            else
            {
                if (transferReturn != LIBUSB_ERROR_TIMEOUT)
                {
                    if (attach)
                    {
                        libusb_reset_device(params.handle);
                        attach = false;
                    }
                    return returnError(transferReturn);
                }
                else
                {
                    if (includeTimeout)
                    {
                        largeBuffer.append("TIMEOUT");
                        frames->appendRange(56);
                    }
                }
            }
            if (attach)
            {
                libusb_reset_device(params.handle);
                attach = false;
            }
            if (progress->isCancelled())
            {
                break;
            }
            progress->setProgress(i, transferNum);
            std::this_thread::sleep_for(std::chrono::milliseconds(transferDelay));
        }
        exitLibusb(true, params);
    }
    else
    {
        return ImportResult::error("Invalid Transfer Type Error, please reselect settings and try again.");
    }

    QSharedPointer<BitContainer> container = BitContainer::create(largeBuffer);      //creating a bit container with all the data in it
    container->setName("USB " + deviceName);                                         //esetting the name you see on the side of hobbits
    QSharedPointer<BitInfo> info = BitInfo::create(container->bits()->sizeInBits()); //creating a bit infor for setting frames
    info->setFrames(frames);                                                         // setting frames on every transfer

    //setting the metadata
    info->setMetadata("Device Name", deviceName);
    info->setMetadata("Device Number", params.deviceNum);
    info->setMetadata("Interface Number", params.interfaceNum);
    info->setMetadata("Alternate Setting Number", params.altSetNum);
    info->setMetadata("Endpoint Number", params.endpointNum);
    info->setMetadata("Endpoint Address", (int)params.endpoint);
    info->setMetadata("Number of Transfers", transferNum);
    info->setMetadata("Transfer Delay Duration", transferDelay);
    info->setMetadata("Transfer Timeout Duration", transferTimeout);
    info->setMetadata("Transfer Type", transferTypeStr);
    info->setMetadata("Transfer Size", transferSize);

    //adding the info to the BitContainer
    container->setInfo(info);

    //returning the bit container
    return ImportResult::result(container, parameters);
}

QSharedPointer<ImportResult> UsbDevice::returnError(int errorCode)
{
    if (errorCode == LIBUSB_ERROR_IO)
    {
        return ImportResult::error("Device IO Error, error reading from the device, please try again. If the problem continues, raise an issue on GitHub.");
    }
    else if (errorCode == LIBUSB_ERROR_INVALID_PARAM)
    {
        return ImportResult::error("Invalid Parameter Error, An invalid device parameter was passed, please check your entry and try again.");
    }
    else if (errorCode == LIBUSB_ERROR_ACCESS)
    {
        return ImportResult::error("Insufficient Permissions Error, Try restarting hobbits in root, or with valid chmod permissions like a+x.");
    }
    else if (errorCode == LIBUSB_ERROR_NO_DEVICE)
    {
        return ImportResult::error("No Device Found Error, Device selected could not be found, try replugging your device and/or restarting hobbits.");
    }
    else if (errorCode == LIBUSB_ERROR_NOT_FOUND)
    {
        return ImportResult::error("Entity Not Found Error, A Device, Interface, Alternate Setting, or Endpoint was not found, check your selection and try again. If the problem continues, raise an issue on GitHub.");
    }
    else if (errorCode == LIBUSB_ERROR_BUSY)
    {
        return ImportResult::error("Device Busy Error, the device you selected is busy with another task, please try again in a bit. If the problem continues, raise an issue on GitHub.");
    }
    else if (errorCode == LIBUSB_ERROR_OVERFLOW)
    {
        return ImportResult::error("Buffer Overflow Error, The buffer used to temporarily store the usb data has overflown, please try again. If the problem continues, raise an issue on GitHub.");
    }
    else if (errorCode == LIBUSB_ERROR_PIPE)
    {
        return ImportResult::error("Pipe Error, The endpoint you selected stopped sending data, please replug your device and try again. If the problem continues, raise an issue on GitHub.");
    }
    else if (errorCode == LIBUSB_ERROR_NO_MEM)
    {
        return ImportResult::error("Out Of Memory Error, Libusb cannot find enough memory to open a device, please close some other applications, and try again. If the problem continues, raise an issue on GitHub.");
    }
    else if (errorCode == LIBUSB_ERROR_NOT_SUPPORTED)
    {
        return ImportResult::error("Not Supported Error, The device you selected is not supported, please choose a different device and try again. If the problem continues, raise an issue on GitHub.");
    }
    else if (errorCode == LIBUSB_ERROR_OTHER)
    {
        return ImportResult::error("Libusb Ran Into an Error in its Processing, please try again. If the problem continues, raise an issue on GitHub.");
    }
    return nullptr;
}

/**
 * @brief The function that would export the bits if the plugin allowed for exporting bits, DO NOT REMOVE
 * as it is needed for the importexport plugin definition to work
 * 
 * 
 * @param container The container to export
 * @param parameters the parameters require for the export
 * @param progress the progressbar and progress checker for the output
 * @return QSharedPointer<ExportResult> - a pointer towards the export result, right now throws an error if its ever called 
 */
QSharedPointer<ExportResult> UsbDevice::exportBits(QSharedPointer<const BitContainer> container,
                                                   const Parameters &parameters,
                                                   QSharedPointer<PluginActionProgress> progress)
{
    return ExportResult::error("Export not implemented");
}
