#ifndef PLUGINSTATEDELEGATE_H
#define PLUGINSTATEDELEGATE_H

#include "hobbits-core_global.h"
#include "pluginstateeditor.h"
#include <QObject>
#include <QJsonValue>
#include <QSize>
#include <QMap>
#include <QVector>
#include <QSharedPointer>

class HOBBITSCORESHARED_EXPORT PluginStateManager
{
public:
    struct Parameter
    {
        QString name;
        QJsonValue::Type type;
        bool optional;
    };

    PluginStateManager(QVector<Parameter> parameters);
    virtual ~PluginStateManager() = default;

    virtual QSharedPointer<PluginStateEditor> createEditor(QSize targetBounds = QSize());
    QList<Parameter> parameters() const;
    bool validate(const QJsonObject &pluginState) const;

protected:
    QMap<QString, Parameter> m_parameterMap;
};

#endif // PLUGINSTATEDELEGATE_H
