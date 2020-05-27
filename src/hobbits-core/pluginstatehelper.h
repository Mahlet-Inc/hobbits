#ifndef PLUGINSTATEHELPER_H
#define PLUGINSTATEHELPER_H

#include <QVariant>
#include <QJsonValue>
#include <QSharedPointer>
#include "hobbits-core_global.h"

class HOBBITSCORESHARED_EXPORT PluginStateHelper
{
public:
    PluginStateHelper();

    class Parameter
    {
    public:
        Parameter(QString name,
                  QJsonValue::Type type,
                  const std::function<bool(QJsonValue)> setInUi,
                  const std::function<QJsonValue()> getFromUi,
                  bool optional) :
            name(name),
            type(type),
            setInUi(setInUi),
            getFromUi(getFromUi),
            optional(optional)
        {

        }

        QString name;
        QJsonValue::Type type;
        std::function<bool(QJsonValue)> setInUi;
        std::function<QJsonValue()> getFromUi;
        bool optional;
    };

    void addParameter(QString name,
                      QJsonValue::Type type,
                      const std::function<bool(QJsonValue)> setInUi,
                      const std::function<QJsonValue()> getFromUi,
                      bool optional=false);

    bool validatePluginState(const QJsonObject &pluginState);
    bool applyPluginStateToUi(const QJsonObject &pluginState);
    QJsonObject getPluginStateFromUi();

private:
    QList<QSharedPointer<Parameter>> m_parameters;
};

#endif // PLUGINSTATEHELPER_H
