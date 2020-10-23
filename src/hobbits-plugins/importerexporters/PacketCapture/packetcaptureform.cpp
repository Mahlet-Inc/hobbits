#include "packetcaptureform.h"
#include "ui_packetcaptureform.h"
#include <QStandardItemModel>
#include <QHostAddress>

enum DevData {
    Name = Qt::UserRole,
    Address = Qt::UserRole + 1,
    Mask = Qt::UserRole + 2
};

PacketCaptureForm::PacketCaptureForm(QSharedPointer<ParameterDelegate> delegate) :
    ui(new Ui::PacketCaptureForm),
    m_paramHelper(new ParameterHelper(delegate))
{
    ui->setupUi(this);

    m_paramHelper->addComboBoxParameter("device_name", ui->cb_device, DevData::Name);
    m_paramHelper->addComboBoxParameter("address", ui->cb_device, DevData::Address);
    m_paramHelper->addSpinBoxIntParameter("timeout", ui->sb_timeout);
    m_paramHelper->addLineEditStringParameter("filter", ui->le_filter);
    m_paramHelper->addSpinBoxIntParameter("buffer_size", ui->sb_packetBufferSize);
    m_paramHelper->addSpinBoxIntParameter("packet_size_limit", ui->sb_maxPacketSize);
    m_paramHelper->addCheckBoxBoolParameter("promiscuous", ui->ck_promiscuous);

    populateDeviceBox();
}

PacketCaptureForm::~PacketCaptureForm()
{
    delete ui;
}

QString PacketCaptureForm::title()
{
    return "Configure Packet Capture";
}

bool PacketCaptureForm::setParameters(QJsonObject parameters)
{
    bool ret = m_paramHelper->applyParametersToUi(parameters);
    if (parameters.contains("max_packets")) {
        ui->cb_limitType->setCurrentText("Packets");
        ui->sb_limit->setValue(parameters.value("max_packets").toInt());
        return ret;
    }
    else if (parameters.contains("max_size")) {
        ui->cb_limitType->setCurrentText("KB");
        ui->sb_limit->setValue(parameters.value("max_size").toInt() / 1000);
        return ret;
    }
    else {
        return false;
    }
}

QJsonObject PacketCaptureForm::parameters()
{
    QJsonObject params = m_paramHelper->getParametersFromUi();
    if (ui->cb_limitType->currentText() == "Packets") {
        params.insert("max_packets", ui->sb_limit->value());
    }
    else if (ui->cb_limitType->currentText() == "KB") {
        params.insert("max_size", ui->sb_limit->value() * 1000);
    }
    return params;
}

bool PacketCaptureForm::isStandaloneDialog()
{
    return true;
}

void PacketCaptureForm::on_pb_beginCapture_clicked()
{
    emit accepted();
}

void PacketCaptureForm::populateDeviceBox()
{
    ui->lb_errorLabel->setVisible(false);
    ui->cb_device->clear();

    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_if_t *dev;
    pcap_if_t *firstDev;

    if (pcap_findalldevs(&dev, errbuf) != 0) {
        ui->lb_errorLabel->setVisible(true);
        ui->lb_errorLabel->setText(errbuf);
        return;
    }
    firstDev = dev;

    auto itemModel = new QStandardItemModel();
    itemModel->setColumnCount(1);
    while(dev) {
        auto address = dev->addresses;
        while (address) {
            QHostAddress addr(address->addr);
            if (addr.protocol() == QAbstractSocket::IPv4Protocol) {
                auto label = QString("%1 %2").arg(dev->name).arg(addr.toString());
                QStandardItem *item = new QStandardItem(label);
                item->setData(dev->name, DevData::Name);
                item->setData(addr.toIPv4Address(), DevData::Address);
                quint32 mask = 0;
                if (address->netmask != nullptr) {
                    QHostAddress netmask(address->netmask);
                    mask = netmask.toIPv4Address();
                }
                item->setData(mask, DevData::Mask);
                itemModel->appendRow(item);
            }
            address = address->next;
        }
        dev = dev->next;
    }
    ui->cb_device->setModel(itemModel);

    pcap_freealldevs(firstDev);
}
