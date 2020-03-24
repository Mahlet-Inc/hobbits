#include "httpdata.h"

HttpData::HttpData() :
    http(nullptr)
{

}

HttpData::~HttpData()
{
    if (http) {
        delete http;
    }
}

ImportExportInterface* HttpData::createDefaultImporterExporter()
{
    return new HttpData();
}

QString HttpData::getName()
{
    return "HTTP Data (REST)";
}

bool HttpData::canExport()
{
    return true;
}

bool HttpData::canImport()
{
    return true;
}

QSharedPointer<BitContainer> HttpData::importBits(QMap<QString, QString> args, QWidget *parent)
{
    Q_UNUSED(args)
    Q_UNUSED(parent)

    if (!http) {
        http = new HttpTransceiver();
    }
    http->setDownloadMode();
    if (http->exec()) {
        auto container = QSharedPointer<BitContainer>(new BitContainer());
        container->setBits(http->getDownloadedData());
        container->setName(http->getUrl().toDisplayString());
        return container;
    }

    return QSharedPointer<BitContainer>();
}

void HttpData::exportBits(QSharedPointer<const BitContainer> container, QMap<QString, QString> args, QWidget *parent)
{
    Q_UNUSED(args)
    Q_UNUSED(parent)

    if (!http) {
        http = new HttpTransceiver();
    }
    http->setUploadMode(container->bits()->getPreviewBytes());
    http->exec();
}
