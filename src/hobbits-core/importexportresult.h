#ifndef IMPORTEXPORTRESULT_H
#define IMPORTEXPORTRESULT_H

#include "bitcontainer.h"
#include <QJsonObject>

class HOBBITSCORESHARED_EXPORT ImportExportResult
{
public:
    ImportExportResult();

    ImportExportResult* setContainer(QSharedPointer<BitContainer> container);
    QSharedPointer<BitContainer> getContainer() const;

    ImportExportResult* setPluginState(QJsonObject pluginState);
    const QJsonObject getPluginState() const;

    static QSharedPointer<ImportExportResult> nullResult();
    static QSharedPointer<ImportExportResult> error(QString error);
    static QSharedPointer<ImportExportResult> create(QSharedPointer<BitContainer> container, QJsonObject pluginState);
    static QSharedPointer<ImportExportResult> create(QJsonObject pluginState);

private:
    QSharedPointer<BitContainer> m_container;
    QJsonObject m_pluginState;
};

#endif // IMPORTEXPORTRESULT_H
