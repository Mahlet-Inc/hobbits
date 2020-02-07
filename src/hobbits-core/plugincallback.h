#ifndef PLUGINCALLBACK_H
#define PLUGINCALLBACK_H

#include <QJsonObject>
#include <QObject>

class PluginCallback : public QObject
{
    Q_OBJECT

public:
    explicit PluginCallback(QObject *parent = nullptr);

signals:
    void runRequested(QString pluginName, QJsonObject pluginState);

public slots:
    void requestRun(QString pluginName, QJsonObject pluginState = QJsonObject());

};

#endif // PLUGINCALLBACK_H
