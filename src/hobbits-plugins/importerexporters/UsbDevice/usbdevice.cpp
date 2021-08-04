#include "usbdevice.h"
#include "usbdeviceimporteditor.h"
#include <libusb-1.0/libusb.h>
#include <iostream>
#include <chrono>
#include <thread>




USBDevice::USBDevice() 
{
    QList<ParameterDelegate::ParameterInfo> importInfos = {
        // TODO: add parameters like {"myparametername", ParameterDelegate::ParameterType::Decimal}
        {"DeviceNum", ParameterDelegate::ParameterType::Integer},
        {"InterfaceNum", ParameterDelegate::ParameterType::Integer},
        {"AltSetNum", ParameterDelegate::ParameterType::Integer},
        {"EndpointNum", ParameterDelegate::ParameterType::Integer},
        {"TransferNum", ParameterDelegate::ParameterType::Integer},
        {"TransferDelay", ParameterDelegate::ParameterType::Integer},
        {"TransferTimeout", ParameterDelegate::ParameterType::Integer},
        {"TransferType", ParameterDelegate::ParameterType::Integer},
        {"TransferSize", ParameterDelegate::ParameterType::Integer},
        {"DeviceName", ParameterDelegate::ParameterType::String}
    };

    m_importDelegate = ParameterDelegate::create(
                    importInfos,
                    [this](const Parameters &parameters) {
                        // TODO: use parameters to describe action better
                        return QString("%1 Import").arg(this->name());
                    },
                    [](QSharedPointer<ParameterDelegate> delegate, QSize size) {
                        Q_UNUSED(size)
                        return new USBDeviceImportEditor(delegate);
                    });

    QList<ParameterDelegate::ParameterInfo> exportInfos = {
        // TODO: add parameters like {"myparametername", ParameterDelegate::ParameterType::Decimal}
    };

     m_exportDelegate = nullptr;
    // ParameterDelegate::create(
    //                 exportInfos,
    //                 [this](const Parameters &parameters) {
    //                     // TODO: use parameters to describe action better
    //                     return QString("%1 Export").arg(this->name());
    //                 },
    //                 [](QSharedPointer<ParameterDelegate> delegate, QSize size) {
    //                     Q_UNUSED(size)
    //                     return new USBDeviceExportEditor(delegate);
    //                 });
}

ImporterExporterInterface* USBDevice::createDefaultImporterExporter()
{
    return new USBDevice();
}

QString USBDevice::name()
{
    return "USB Device";
}

QString USBDevice::description()
{
    // TODO: create actual description
    return "Describes what USBDevice does";
}

QStringList USBDevice::tags()
{
    // TODO: add relevant tags
    return {"Generic"};
}

bool USBDevice::canExport()
{
    return false;
}

bool USBDevice::canImport()
{
    return true;
}

QSharedPointer<ParameterDelegate>  USBDevice::importParameterDelegate()
{
    return m_importDelegate;
}

QSharedPointer<ParameterDelegate>  USBDevice::exportParameterDelegate()
{
    return m_exportDelegate;
}


void USBDevice::setupLibusb(){
    /*initialize libusb,
    get device list,
    get device,
    close device list,
    get config descriptor,
    get endpoint,
    setup device handle,
    set alt setting,
    */
   int r = libusb_init(&m_ctx);
    if(r < 0){
        std::runtime_error libusb_init_error("Error while trying to initialize Libusb");
		throw libusb_init_error;
    }
   libusb_set_option(m_ctx, LIBUSB_OPTION_LOG_LEVEL, 4 );
   libusb_get_device_list(m_ctx, &m_devs);
   m_dev = m_devs[m_deviceNum];
   libusb_free_device_list(m_devs, m_deviceNum);
   libusb_get_active_config_descriptor(m_dev, &m_config);

   const libusb_interface *inter = &m_config->interface[m_interfaceNum];
   const libusb_interface_descriptor *interdesc = &inter->altsetting[m_altSetNum];
   const libusb_endpoint_descriptor *epdesc = &interdesc->endpoint[m_endpointNum];

   m_endpoint = epdesc->bEndpointAddress;
   
   libusb_open(m_dev, &m_handle);
   libusb_set_interface_alt_setting(m_handle, m_interfaceNum, m_altSetNum);
}

void USBDevice::exitLibusb(){
    /*free config descriptor,
    free device handle
    exit libusb session*/
    libusb_close(m_handle);
    libusb_free_config_descriptor(m_config);
    libusb_exit(m_ctx);
}


QSharedPointer<ImportResult> USBDevice::importBits(const Parameters &parameters,
                                                      QSharedPointer<PluginActionProgress> progress)
{
    QStringList invalidations = m_importDelegate->validate(parameters);
    if (!invalidations.isEmpty()) {
        return ImportResult::error(QString("Invalid parameters passed to %1:\n%2").arg(name()).arg(invalidations.join("\n")));
    }

    m_deviceNum = parameters.value("DeviceNum").toInt();
    m_interfaceNum = parameters.value("InterfaceNum").toInt();
    m_altSetNum = parameters.value("AltSetNum").toInt();
    m_endpointNum = parameters.value("EndpointNum").toInt();
    int transferNum = parameters.value("TransferNum").toInt();
    int transferDelay = parameters.value("TransferDelay").toInt();
    int transferTimout = parameters.value("TransferTimeout").toInt();
    int transferType = parameters.value("TransferType").toInt();
    int transferSize = parameters.value("TransferSize").toInt();
    QString deviceName = parameters.value("DeviceName").toString();
    QByteArray largeBuffer;
    int actualLength;
    unsigned char smallBuffer[transferSize];
    bool attach;

    switch (transferType)
    {
    case 0:
        return ImportResult::error("Control Transfer Endpoints Not Supported");
        break;

    case 1: 
        return ImportResult::error("Isochronous Transfer Endpoints Not Supported");
        break;

    case 2: 
        /*setupLibusb()
        in loop of numTransfers:
            dettach kernel driver
            initiate bulk transfer
            recieve transfer
            reset the device
            append the data to a hobbits bitcontainer
            check if the action has been cancelled
            update the progressbar
            wait the transferDelay duration
        exitLibusb()
        */

        setupLibusb();
        for(int i = 0; i < transferNum; i++){
            if(libusb_kernel_driver_active(m_handle, m_interfaceNum) == 1){
                attach = true;
                libusb_detach_kernel_driver(m_handle, m_interfaceNum);
            }
            int transferReturn = libusb_bulk_transfer(m_handle, m_endpoint, smallBuffer,(int)sizeof(smallBuffer), &actualLength, transferTimout);
            if(transferReturn == 0 && actualLength != 0){
                for(int j = 0; j < (int)sizeof(smallBuffer); j++){
                    largeBuffer.append(smallBuffer[j]);
                }   
            }else{
                if(transferReturn == LIBUSB_ERROR_TIMEOUT){
                    continue;
                }else{
                    if(transferReturn != LIBUSB_ERROR_TIMEOUT){
                        if(attach){
                            libusb_reset_device(m_handle);
                            attach = false;
                        }
                        return ImportResult::error("Transfer Error, Code: " + QString::number(transferReturn) + " " + libusb_error_name(transferReturn) + ": " + libusb_strerror(transferReturn));
                    }else{
                        largeBuffer.append("TIMEOUT");
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
        exitLibusb();
        break;
    case 3:
        /*setupLibusb()
        create data buffer
        in loop of numTransfers:
            dettach kernel driver
            initiate interrupt transfer
            recieve transfer
            reset the device
            append the data to a hobbits bitcontainer
            wait the transferDelay duration
        exitLibusb()
        */
       setupLibusb();
        for(int i = 0; i < transferNum; i++){
            if(libusb_kernel_driver_active(m_handle, m_interfaceNum) == 1){
                attach = true;
                libusb_detach_kernel_driver(m_handle, m_interfaceNum);
            }
            int transferReturn = libusb_interrupt_transfer(m_handle, m_endpoint, smallBuffer,(int)sizeof(smallBuffer), &actualLength, transferTimout);
            if(transferReturn == 0 && actualLength != 0){
                for(int j = 0; j < (int)sizeof(smallBuffer); j++){
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
                    largeBuffer.append("TIMEOUT");
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
        exitLibusb();
        break;
    
    default:
        break;
    }

    QSharedPointer<BitContainer> container = BitContainer::create(largeBuffer);
    container->setName("USB Device " + deviceName);

    return ImportResult::result(container, parameters);
}

QSharedPointer<ExportResult> USBDevice::exportBits(QSharedPointer<const BitContainer> container,
                                                      const Parameters &parameters,
                                                      QSharedPointer<PluginActionProgress> progress)
{
    return ExportResult::error("Export not implemented");
}
