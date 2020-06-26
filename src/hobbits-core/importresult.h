#ifndef IMPORTRESULT_H
#define IMPORTRESULT_H

#include "bitcontainer.h"
#include <QJsonObject>

class HOBBITSCORESHARED_EXPORT ImportResult
{
public:
    ImportResult();

    ImportResult* setContainer(QSharedPointer<BitContainer> container);
    QSharedPointer<BitContainer> getContainer() const;

    ImportResult* setPluginState(QJsonObject pluginState);
    const QJsonObject pluginState() const;

    bool hasEmptyState() const;
    QString errorString() const;

    static QSharedPointer<ImportResult> nullResult();
    static QSharedPointer<ImportResult> error(QString error);
    static QSharedPointer<ImportResult> result(QSharedPointer<BitContainer> container, QJsonObject pluginState);

private:
    QSharedPointer<BitContainer> m_container;
    QJsonObject m_pluginState;
};

#endif // IMPORTRESULT_H
