#ifndef PLUGINSTATEEDITOR_H
#define PLUGINSTATEEDITOR_H

#include "hobbits-core_global.h"
#include <QObject>
#include <QJsonValue>

class HOBBITSCORESHARED_EXPORT PluginStateEditor
{
public:
    virtual ~PluginStateEditor() = default;

    virtual QWidget* widget() = 0;
    virtual bool prompt() = 0;

    virtual QJsonObject state() const = 0;
    virtual bool setState(const QJsonObject &pluginState) = 0;
};


#endif // PLUGINSTATEEDITOR_H
