#include "parameterhelper.h"
#include <QJsonObject>

ParameterHelper::ParameterHelper(QSharedPointer<ParameterDelegate> delegate) :
    m_delegate(delegate)
{

}
void ParameterHelper::addParameter(QString name,
                  const std::function<bool(QJsonValue)> setInUi,
                  const std::function<QJsonValue()> getFromUi)
{
    m_parameterInfos.append(QSharedPointer<ParameterInfoUi>(new ParameterInfoUi(m_delegate->getInfo(name), setInUi, getFromUi)));
}

void ParameterHelper::addCheckBoxBoolParameter(QString name, QCheckBox* checkBox)
{
    addParameter(name, [checkBox](QJsonValue value) {
        checkBox->setChecked(value.toBool(false));
        return true;
    }, [checkBox]() {
        return QJsonValue(checkBox->checkState() == Qt::Checked);
    });
}

void ParameterHelper::addSpinBoxIntParameter(QString name, QSpinBox* spinBox)
{
    addParameter(name, [spinBox](QJsonValue value) {
        spinBox->setValue(value.toInt());
        return true;
    }, [spinBox]() {
        return QJsonValue(spinBox->value());
    });
}

void ParameterHelper::addLineEditStringParameter(QString name, QLineEdit* lineEdit)
{
    addParameter(name, [lineEdit](QJsonValue value) {
        lineEdit->setText(value.toString());
        return true;
    }, [lineEdit]() {
        return QJsonValue(lineEdit->text());
    });
}


void ParameterHelper::addTextEditStringParameter(QString name, QPlainTextEdit* textEdit)
{
    addParameter(name, [textEdit](QJsonValue value) {
        textEdit->setPlainText(value.toString());
        return true;
    }, [textEdit]() {
        return QJsonValue(textEdit->toPlainText());
    });
}

void ParameterHelper::addComboBoxParameter(QString name, QComboBox* comboBox, int role)
{
    auto type = m_delegate->getInfo(name).type;
    addParameter(name, [comboBox, type, role](QJsonValue value) {
        int index = -1;
        if (type == QJsonValue::Bool) {
            index = comboBox->findData(value.toBool(), role);
        }
        else if (type == QJsonValue::String) {
            index = comboBox->findData(value.toString(), role);
        }
        else if (type == QJsonValue::Double) {
            index = comboBox->findData(value.toDouble(), role);
        }
        if (index < 0) {
            return false;
        }
        comboBox->setCurrentIndex(index);
        return true;
    }, [comboBox, type, role]() {
        if (type == QJsonValue::Bool) {
            return QJsonValue(comboBox->currentData(role).toBool());
        }
        else if (type == QJsonValue::String) {
            return QJsonValue(comboBox->currentData(role).toString());
        }
        else if (type == QJsonValue::Double) {
            return QJsonValue(comboBox->currentData(role).toDouble());
        }
        return QJsonValue();
    });
}

bool ParameterHelper::applyParametersToUi(const QJsonObject &parameters)
{
    if (!m_delegate->validate(parameters)) {
        return false;
    }

    for (auto param : m_parameterInfos) {
        if (!parameters.contains(param->info().name)) {
            continue;
        }
        if (!param->setInUi(parameters.value(param->info().name))) {
            if (param->info().optional) {
                continue;
            }
            return false;
        }
    }

    return true;
}

QJsonObject ParameterHelper::getParametersFromUi()
{
    QJsonObject pluginState;
    for (auto param : m_parameterInfos) {
        QJsonValue value = param->getFromUi();
        if (value.isUndefined()) {
            if (param->info().optional) {
                continue;
            }
            return QJsonObject();
        }
        pluginState.insert(param->info().name, value);
    }
    return pluginState;
}
