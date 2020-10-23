#include "httpdata.h"
#include <QMessageBox>
#include "httpexportform.h"
#include "httpimportform.h"

HttpData::HttpData()
{
    QList<ParameterDelegate::ParameterInfo> importInfos = {
        {"url", QJsonValue::String},
        {"verb", QJsonValue::String}
    };

    m_importDelegate = QSharedPointer<ParameterDelegateUi>(
                new ParameterDelegateUi(
                    importInfos,
                    [](const QJsonObject &parameters) {
                        if (parameters.contains("url")) {
                            QString url = parameters.value("url").toString();
                            if (url.size() > 16) {
                                url.truncate(12);
                                url += "...";
                            }
                            return QString("Import HTTP '%1'").arg(url);
                        }
                        else {
                            return QString();
                        }
                    },
                    [](QSharedPointer<ParameterDelegate> delegate, QSize size) {
                        Q_UNUSED(size)
                        return new HttpImportForm(delegate);
                    }));


    QList<ParameterDelegate::ParameterInfo> exportInfos = {
        {"url", QJsonValue::String},
        {"formdataname", QJsonValue::String},
        {"verb", QJsonValue::String}
    };
    m_exportDelegate = QSharedPointer<ParameterDelegateUi>(
                new ParameterDelegateUi(
                    exportInfos,
                    [](const QJsonObject &parameters) {
                        if (parameters.contains("url")) {
                            QString url = parameters.value("url").toString();
                            if (url.size() > 16) {
                                url.truncate(12);
                                url += "...";
                            }
                            return QString("Export HTTP '%1'").arg(url);
                        }
                        else {
                            return QString();
                        }
                    },
                    [](QSharedPointer<ParameterDelegate> delegate, QSize size) {
                        Q_UNUSED(size)
                        return new HttpExportForm(delegate);
                    }));
}

ImporterExporterInterface* HttpData::createDefaultImporterExporter()
{
    return new HttpData();
}

QString HttpData::name()
{
    return "HTTP Data (REST)";
}

QString HttpData::description()
{
    return "HTTP Importing and Exporting";
}

QStringList HttpData::tags()
{
    return {"Generic", "Network"};
}

bool HttpData::canExport()
{
    return true;
}

bool HttpData::canImport()
{
    return true;
}

QSharedPointer<ParameterDelegate> HttpData::importParameterDelegate()
{
    return m_importDelegate;
}

QSharedPointer<ParameterDelegate> HttpData::exportParameterDelegate()
{
    return m_exportDelegate;
}

QSharedPointer<ImportResult> HttpData::importBits(QJsonObject parameters,
                                                  QSharedPointer<PluginActionProgress> progress)
{
    if (!m_importDelegate->validate(parameters)) {
        return ImportResult::error("Invalid parameters passed to HTTP Import");
    }

    return HttpImportForm::importData(parameters, progress);
}

QSharedPointer<ExportResult> HttpData::exportBits(QSharedPointer<const BitContainer> container,
                                                  QJsonObject parameters,
                                                  QSharedPointer<PluginActionProgress> progress)
{
    // TODO: make this part of post-result reporting
//    if (container->bits()->sizeInBytes() > 10000000) {
//        QMessageBox::StandardButton reply;
//        reply = QMessageBox::question(
//                nullptr,
//                "Data Truncation Warning",
//                QString("Uploaded data will be truncated at 10MB. Do you want to proceed anyways?"),
//                QMessageBox::Yes | QMessageBox::No);
//        if (reply != QMessageBox::Yes) {
//            return ExportResult::nullResult();
//        }
//    }

    if (!m_exportDelegate->validate(parameters)) {
        return ExportResult::error("Invalid parameters passed to HTTP Import");
    }

    return HttpExportForm::exportData(container->bits()->readBytes(0, 25000000), parameters, progress);
}
