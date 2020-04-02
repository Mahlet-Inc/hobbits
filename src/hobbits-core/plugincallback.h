#ifndef PLUGINCALLBACK_H
#define PLUGINCALLBACK_H

#include <QJsonObject>
#include <QObject>
#include "displayhandle.h"
#include "hobbits-core_global.h"

class HOBBITSCORESHARED_EXPORT PluginCallback : public QObject
{
    Q_OBJECT

public:
    explicit PluginCallback(QSharedPointer<DisplayHandle> displayHandle, QObject *parent = nullptr);

    QSharedPointer<DisplayHandle> getDisplayHandle();

signals:
    void operatorRunRequested(QString pluginName, QJsonObject pluginState);
    void analyzerRunRequested(QString pluginName, QJsonObject pluginState);
    void operatorStateChanged(QString pluginName);

public slots:
    void sendOperatorStateChanged(QString pluginName);
    void requestOperatorRun(QString pluginName, QJsonObject pluginState = QJsonObject());
    void requestAnalyzerRun(QString pluginName, QJsonObject pluginState = QJsonObject());

private:
    QSharedPointer<DisplayHandle> m_displayHandle;
};

#endif // PLUGINCALLBACK_H
