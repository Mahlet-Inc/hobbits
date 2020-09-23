#include "capturedialog.h"
#include "ui_capturedialog.h"
#include <QMessageBox>
#include <QStandardItem>
#include <QHostAddress>
#include <QMetaObject>
#include <QtConcurrent/QtConcurrentRun>

enum DevData {
    Name = Qt::UserRole,
    Address = Qt::UserRole + 1,
    Mask = Qt::UserRole + 2
};

CaptureDialog::CaptureDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::CaptureDialog)
{
    ui->setupUi(this);
    populateDeviceBox();

    setWindowTitle("Packet Capture");
}

CaptureDialog::~CaptureDialog()
{
    delete ui;
}

void CaptureDialog::captureLoop(CaptureDialog *capture)
{
    int packets = 0;
    if (capture->ui->cb_limitType->currentText() == "Packets") {
        packets = capture->ui->sb_limit->value();
    }

    void *arg = capture;
    int loopRet = pcap_loop(capture->m_handle, packets, &CaptureDialog::processPacket, static_cast<u_char*>(arg));
    if (loopRet == PCAP_ERROR) {
        QMetaObject::invokeMethod(capture, "finalizeCapture", Qt::QueuedConnection, Q_ARG(QString, "Error during packet capture"));
    }
    else {
        QMetaObject::invokeMethod(capture, "finalizeCapture", Qt::QueuedConnection);
    }
}

void CaptureDialog::processPacket(u_char *args, const pcap_pkthdr *header, const u_char *packet)
{
    CaptureDialog * capture = static_cast<CaptureDialog*>(static_cast<void*>(args));
    capture->m_file.write(reinterpret_cast<const char*>(packet), header->caplen);
    capture->m_packetSizes->appendRange(header->caplen * 8);

    QMetaObject::invokeMethod(capture, "setProgress", Qt::QueuedConnection, Q_ARG(int, int(capture->m_packetSizes->size())), Q_ARG(int, int(capture->m_packetSizes->getValueCount() / 8)));
    if (capture->ui->cb_limitType->currentText() == "KB") {
        if (capture->m_file.size() >= capture->ui->sb_limit->value() * 1000) {
            QMetaObject::invokeMethod(capture, "endLoop", Qt::QueuedConnection);
        }
    }
}

void CaptureDialog::populateDeviceBox()
{
    ui->cb_device->clear();

    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_if_t *dev;
    pcap_if_t *firstDev;

    if (pcap_findalldevs(&dev, errbuf) != 0) {
        ui->lb_packets->setText(errbuf);
        ui->lb_kilobytes->setText("");
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

    checkCurrDevice();
}

void CaptureDialog::checkCurrDevice()
{
    if (ui->cb_device->currentText().isEmpty()) {
        ui->pb_beginCapture->setEnabled(false);
        return;
    }
    ui->pb_beginCapture->setEnabled(true);
}

void CaptureDialog::endLoop()
{
    if (m_handle) {
        pcap_breakloop(m_handle);
    }
}

void CaptureDialog::finalizeCapture(QString error)
{
    if (!error.isEmpty()) {
        QMessageBox msg(this);
        msg.setWindowTitle("Packet Capture Error");
        msg.setText(error);
        msg.setDefaultButton(QMessageBox::Ok);
        msg.exec();
    }

    if (m_file.size() > 0) {
        m_file.seek(0);
        m_container = BitContainer::create(&m_file, m_packetSizes->getValueCount());
        auto info = BitInfo::create(m_container->bits()->sizeInBits());
        info->setFrames(m_packetSizes);
        m_container->setInfo(info);
        m_container->setName("Packet Capture");
        m_file.close();
        this->accept();
    }

    m_file.close();
    endCapture();
}

void CaptureDialog::endCapture(QString error)
{
    if (!error.isEmpty()) {
        QMessageBox msg(this);
        msg.setWindowTitle("Capture Initialization Error");
        msg.setText(error);
        msg.setDefaultButton(QMessageBox::Ok);
        msg.exec();
    }

    if (m_handle) {
        pcap_close(m_handle);
        m_handle = nullptr;
    }

    ui->configLayout->setEnabled(true);
    ui->pb_beginCapture->setEnabled(true);
    ui->pb_stopCapture->setEnabled(false);
}

void CaptureDialog::setProgress(int packets, int bytes)
{
    ui->lb_packets->setText(QString("%1 packets").arg(packets));
    ui->lb_kilobytes->setText(QString("%1 KB").arg(double(bytes)/1000.0, 1, 'f', 3));
}

void CaptureDialog::on_pb_beginCapture_clicked()
{
    ui->configLayout->setEnabled(false);
    ui->pb_beginCapture->setEnabled(false);
    ui->pb_stopCapture->setEnabled(true);

    char errbuf[PCAP_ERRBUF_SIZE];

    QString devName = ui->cb_device->currentData(DevData::Name).toString();
    bpf_u_int32 net = ui->cb_device->currentData(DevData::Address).toUInt();
    bpf_u_int32 mask = ui->cb_device->currentData(DevData::Mask).toUInt();

    int promiscuous = 0;
    if (ui->ck_promiscuous->isChecked()) {
        promiscuous = 1;
    }
    int timeout = 0;
    if (ui->ck_timeout->isChecked()) {
        timeout = ui->sb_timeout->value();
    }
    m_handle = pcap_open_live(devName.toStdString().c_str(), BUFSIZ, promiscuous, timeout, errbuf);
    if (m_handle == nullptr) {
        endCapture("Failed to open pcap handle:\n" + QString(errbuf));
        return;
     }

    if (!ui->le_filter->text().isEmpty()) {
        struct bpf_program filter;
        if (pcap_compile(m_handle, &filter, ui->le_filter->text().toStdString().c_str(), 1, net) == -1) {
            endCapture("Failed to compile filter:\n" + QString(errbuf));
            return;
        }
        if (pcap_setfilter(m_handle, &filter) == -1) {
            endCapture("Failed to apply filter:\n" + QString(errbuf));
            return;
        }
    }

    if (!m_file.open()) {
        endCapture("Failed to open a temporary file buffer for packets");
    }
    m_file.resize(0);
    m_packetSizes = RangeSequence::createEmpty();

    QtConcurrent::run(&CaptureDialog::captureLoop, this);
}

void CaptureDialog::on_pb_stopCapture_clicked()
{
    endLoop();
}

QSharedPointer<BitContainer> CaptureDialog::container() const
{
    return m_container;
}
