#include "pluginstatehelper.h"
#include <QJsonObject>

PluginStateHelper::PluginStateHelper()
{

}
void PluginStateHelper::addParameter(QString name,
                  QJsonValue::Type type,
                  const std::function<bool(QJsonValue)> setInUi,
                  const std::function<QJsonValue()> getFromUi,
                  bool optional)
{
    m_parameters.append(QSharedPointer<Parameter>(new Parameter(name, type, setInUi, getFromUi, optional)));
}

void PluginStateHelper::addCheckBoxBoolParameter(QString name, const std::function<QCheckBox*()> checkBoxGetter, bool optional)
{
    addParameter(name, QJsonValue::Bool, [checkBoxGetter](QJsonValue value) {
        checkBoxGetter()->setChecked(value.toBool(false));
        return true;
    }, [checkBoxGetter]() {
        return QJsonValue(checkBoxGetter()->checkState() == Qt::Checked);
    }, optional);
}

void PluginStateHelper::addSpinBoxIntParameter(QString name, const std::function<QSpinBox*()> spinBoxGetter, bool optional)
{
    addParameter(name, QJsonValue::Double, [spinBoxGetter](QJsonValue value) {
        spinBoxGetter()->setValue(value.toInt());
        return true;
    }, [spinBoxGetter]() {
        return QJsonValue(spinBoxGetter()->value());
    }, optional);
}

void PluginStateHelper::addLineEditStringParameter(QString name, const std::function<QLineEdit*()> lineEditGetter, bool optional)
{
    addParameter(name, QJsonValue::String, [lineEditGetter](QJsonValue value) {
        lineEditGetter()->setText(value.toString());
        return true;
    }, [lineEditGetter]() {
        return QJsonValue(lineEditGetter()->text());
    }, optional);
}


void PluginStateHelper::addTextEditStringParameter(QString name, const std::function<QPlainTextEdit*()> textEditGetter, bool optional)
{
    addParameter(name, QJsonValue::String, [textEditGetter](QJsonValue value) {
        textEditGetter()->setPlainText(value.toString());
        return true;
    }, [textEditGetter]() {
        return QJsonValue(textEditGetter()->toPlainText());
    }, optional);
}

bool PluginStateHelper::validatePluginState(const QJsonObject &pluginState)
{
    if (pluginState.isEmpty()) {
        return false;
    }

    for (auto param : m_parameters) {
        if (!pluginState.contains(param->name)) {
            if (!param->optional) {
                return false;
            }
        }
        else if (param->type != pluginState.value(param->name).type()) {
            return false;
        }
    }
    return true;
}

bool PluginStateHelper::applyPluginStateToUi(const QJsonObject &pluginState)
{
    if (!validatePluginState(pluginState)) {
        return false;
    }

    for (auto param : m_parameters) {
        if (!pluginState.contains(param->name)) {
            continue;
        }
        if (!param->setInUi(pluginState.value(param->name))) {
            return false;
        }
    }

    return true;
}

QJsonObject PluginStateHelper::getPluginStateFromUi()
{
    QJsonObject pluginState;
    for (auto param : m_parameters) {
        QJsonValue value = param->getFromUi();
        if (value.isUndefined()) {
            return QJsonObject();
        }
        pluginState.insert(param->name, value);
    }
    return pluginState;
}
