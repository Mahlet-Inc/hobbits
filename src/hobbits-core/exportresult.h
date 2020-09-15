#ifndef EXPORTRESULT_H
#define EXPORTRESULT_H

#include "bitcontainer.h"
#include <QJsonObject>

class HOBBITSCORESHARED_EXPORT ExportResult
{
public:
    ExportResult();

    ExportResult* setPluginState(QJsonObject pluginState);
    const QJsonObject pluginState() const;

    bool hasEmptyState() const;
    QString errorString() const;

    static QSharedPointer<ExportResult> nullResult();
    static QSharedPointer<ExportResult> error(QString error);
    static QSharedPointer<ExportResult> result(QJsonObject pluginState);

private:
    QJsonObject m_pluginState;
};

#endif // EXPORTRESULT_H
