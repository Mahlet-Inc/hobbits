#include "plugincallback.h"

PluginCallback::PluginCallback(QSharedPointer<DisplayHandle> displayHandle, QObject *parent) :
    QObject(parent),
    m_displayHandle(displayHandle)
{

}
QSharedPointer<DisplayHandle> PluginCallback::getDisplayHandle()
{
    return m_displayHandle;
}

void PluginCallback::requestAnalyzerRun(QString pluginName, QJsonObject pluginState)
{
    emit analyzerRunRequested(pluginName, pluginState);
}

void PluginCallback::requestOperatorRun(QString pluginName, QJsonObject pluginState)
{
    emit operatorRunRequested(pluginName, pluginState);
}

void PluginCallback::sendOperatorStateChanged(QString pluginName)
{
    emit operatorStateChanged(pluginName);
}
