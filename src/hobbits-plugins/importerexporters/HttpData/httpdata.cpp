#include "httpdata.h"
#include <QMessageBox>
#include "httpexportform.h"
#include "httpimportform.h"

HttpData::HttpData()
{
    QList<ParameterDelegate::ParameterInfo> importInfos = {
        {"url", ParameterDelegate::ParameterType::String},
        {"verb", ParameterDelegate::ParameterType::String}
    };

    m_importDelegate = ParameterDelegate::create(
                    importInfos,
                    [](const Parameters &parameters) {
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
                    });


    QList<ParameterDelegate::ParameterInfo> exportInfos = {
        {"url", ParameterDelegate::ParameterType::String},
        {"formdataname", ParameterDelegate::ParameterType::String},
        {"verb", ParameterDelegate::ParameterType::String}
    };
    m_exportDelegate = ParameterDelegate::create(
                    exportInfos,
                    [](const Parameters &parameters) {
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
                    });
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

QSharedPointer<ImportResult> HttpData::importBits(const Parameters &parameters,
                                                  QSharedPointer<PluginActionProgress> progress)
{
    QStringList invalidations = m_importDelegate->validate(parameters);
    if (!invalidations.isEmpty()) {
        return ImportResult::error(QString("Invalid parameters passed to %1:\n%2").arg(name()).arg(invalidations.join("\n")));
    }

    return HttpImportForm::importData(parameters, progress);
}

QSharedPointer<ExportResult> HttpData::exportBits(QSharedPointer<const BitContainer> container,
                                                  const Parameters &parameters,
                                                  QSharedPointer<PluginActionProgress> progress)
{
    QStringList invalidations = m_exportDelegate->validate(parameters);
    if (!invalidations.isEmpty()) {
        return ExportResult::error(QString("Invalid parameters passed to %1:\n%2").arg(name()).arg(invalidations.join("\n")));
    }

    return HttpExportForm::exportData(container->bits()->readBytes(0, 25000000), parameters, progress);
}
