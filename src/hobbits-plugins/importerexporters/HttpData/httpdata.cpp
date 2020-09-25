#include "httpdata.h"
#include <QMessageBox>

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

QString HttpData::getImportLabelForState(QJsonObject pluginState)
{
    if (pluginState.contains("url")) {
        QString url = pluginState.value("url").toString();
        return QString("Import from %1").arg(url);
    }
    return "";
}

QString HttpData::getExportLabelForState(QJsonObject pluginState)
{
    Q_UNUSED(pluginState)
    return "";
}

QSharedPointer<ImportResult> HttpData::importBits(QJsonObject pluginState)
{
    if (!http) {
        http = new HttpTransceiver();
    }
    http->setDownloadMode();

    if (pluginState.contains("url")) {
        http->setUrl(QUrl(pluginState.value("url").toString()));
    }

    if (http->exec()) {
        auto container = BitContainer::create(http->getDownloadedData());
        container->setName(http->getUrl().toDisplayString());

        pluginState.remove("url");
        pluginState.insert("url", http->getUrl().toDisplayString());

        return ImportResult::result(container, pluginState);
    }

    return ImportResult::nullResult();
}

QSharedPointer<ExportResult> HttpData::exportBits(QSharedPointer<const BitContainer> container, QJsonObject pluginState)
{
    if (!http) {
        http = new HttpTransceiver();
    }

    if (container->bits()->sizeInBytes() > 10000000) {
        QMessageBox::StandardButton reply;
        reply = QMessageBox::question(
                nullptr,
                "Data Truncation Warning",
                QString("Uploaded data will be truncated at 10MB. Do you want to proceed anyways?"),
                QMessageBox::Yes | QMessageBox::No);
        if (reply != QMessageBox::Yes) {
            return ExportResult::nullResult();
        }
    }

    http->setUploadMode(container->bits()->readBytes(0, 25000000));

    if (pluginState.contains("url")) {
        http->setUrl(QUrl(pluginState.value("url").toString()));
    }

    http->exec();

    pluginState.remove("url");
    pluginState.insert("url", http->getUrl().toDisplayString());

    return ExportResult::result(pluginState);
}
