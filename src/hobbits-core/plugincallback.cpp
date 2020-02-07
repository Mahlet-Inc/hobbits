#include "plugincallback.h"

PluginCallback::PluginCallback(QObject *parent) :
    QObject(parent)
{

}

void PluginCallback::requestRun(QString pluginName, QJsonObject pluginState)
{
    emit runRequested(pluginName, pluginState);
}
