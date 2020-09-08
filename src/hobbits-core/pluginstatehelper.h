#ifndef PLUGINSTATEHELPER_H
#define PLUGINSTATEHELPER_H

#include <QVariant>
#include <QJsonValue>
#include <QSharedPointer>
#include <QCheckBox>
#include <QSpinBox>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QComboBox>
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

    void addCheckBoxBoolParameter(QString name,
                      const std::function<QCheckBox*()> checkBoxGetter,
                      bool optional=false);

    void addSpinBoxIntParameter(QString name,
                      const std::function<QSpinBox*()> spinBoxGetter,
                      bool optional=false);

    void addLineEditStringParameter(QString name,
                      const std::function<QLineEdit*()> lineEditGetter,
                      bool optional=false);

    void addTextEditStringParameter(QString name,
                      const std::function<QPlainTextEdit*()> textEditGetter,
                      bool optional=false);

    void addComboBoxParameter(QString name,
                      const std::function<QComboBox*()> comboBoxGetter,
                      QJsonValue::Type type,
                      bool optional=false, int role = Qt::UserRole);

    bool validatePluginState(const QJsonObject &pluginState);
    bool applyPluginStateToUi(const QJsonObject &pluginState);
    QJsonObject getPluginStateFromUi();

private:
    QList<QSharedPointer<Parameter>> m_parameters;
};

#endif // PLUGINSTATEHELPER_H
