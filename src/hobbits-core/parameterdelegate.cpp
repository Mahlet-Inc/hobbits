#include "parameterdelegate.h"
#include <QJsonObject>
#include <QJsonArray>

ParameterDelegate::ParameterDelegate(QList<ParameterDelegate::ParameterInfo> parameters,
                                     std::function<QString(const QJsonObject&)> actionDescriber) :
    ParameterDelegate(parameters,
                      actionDescriber,
                      [](QSharedPointer<ParameterDelegate> delegate, QSize targetBounds){
        Q_UNUSED(delegate)
        Q_UNUSED(targetBounds)
        return nullptr;
    })
{
}

ParameterDelegate::ParameterDelegate(QList<ParameterDelegate::ParameterInfo> parameters,
                                     std::function<QString (const QJsonObject &)> actionDescriber,
                                     std::function<AbstractParameterEditor *(QSharedPointer<ParameterDelegate>, QSize)> editorCreator) :
    m_actionDescriber(actionDescriber),
    m_editorCreator(editorCreator)
{
    for (auto parameter : parameters) {
        m_parameterMap.insert(parameter.name, parameter);
    }
}

QSharedPointer<ParameterDelegate> ParameterDelegate::create(
        QList<ParameterDelegate::ParameterInfo> parameterInfos,
        std::function<QString (const QJsonObject &)> actionDescriber,
        std::function<AbstractParameterEditor *(QSharedPointer<ParameterDelegate>, QSize)> editorCreator)
{
    return QSharedPointer<ParameterDelegate>(new ParameterDelegate(parameterInfos, actionDescriber, editorCreator));
}

AbstractParameterEditor* ParameterDelegate::createEditor(QSize targetBounds)
{
    return m_editorCreator(sharedFromThis(), targetBounds);
}

bool ParameterDelegate::jsonTypeCompatible(QJsonValue::Type jsonType, ParameterDelegate::ParameterType type) {
    if (jsonType == QJsonValue::Double) {
        return (type == ParameterType::Decimal) || (type == ParameterType::Integer);
    }
    else if (jsonType == QJsonValue::String) {
        return type == ParameterType::String;
    }
    else if (jsonType == QJsonValue::Bool) {
        return type == ParameterType::Boolean;
    }
    else if (jsonType == QJsonValue::Array) {
        return type == ParameterType::Array;
    }
    else if (jsonType == QJsonValue::Object) {
        return type == ParameterType::Object;
    }
    return false;
}

QList<ParameterDelegate::ParameterInfo> ParameterDelegate::parameterInfos() const
{
    return m_parameterMap.values();
}

ParameterDelegate::ParameterInfo ParameterDelegate::getInfo(QString name) const
{
    return m_parameterMap.value(name);
}

QStringList ParameterDelegate::validate(const QJsonObject &parameters) const
{
    return validateAgainstInfos(parameters, parameterInfos());
}

QString ParameterDelegate::actionDescription(const QJsonObject &parameters) const
{
    if (!validate(parameters).isEmpty()) {
        return QString();
    }

    return m_actionDescriber(parameters);
}

QStringList ParameterDelegate::validateAgainstInfos(const QJsonObject &parameters, QList<ParameterDelegate::ParameterInfo> infos)
{
    QStringList invalidations;

    for (auto param : infos) {
        if (!parameters.contains(param.name)) {
            if (!param.optional) {
                invalidations.append(QString("Missing required parameter '%1'.").arg(param.name));
                continue;
            }
        }
        else if (!jsonTypeCompatible(parameters.value(param.name).type(), param.type)) {
            invalidations.append(QString("Value of provided parameter '%1' is wrong type.").arg(param.name));
            continue;
        }

        if (param.type == ParameterType::Array) {
            QJsonArray array = parameters.value(param.name).toArray();
            if (array.isEmpty() && !param.optional) {
                invalidations.append(QString("Required array parameter '%1' is empty.").arg(param.name));
                continue;
            }
            for (QJsonValueRef value: array) {
                if (!value.isObject()) {
                    invalidations.append(QString("Array parameter '%1' has invalid value '%2'.").arg(param.name).arg(value.toString()));
                }
                else {
                    invalidations.append(validateAgainstInfos(value.toObject(), param.subInfos));
                }
            }
        }
        else if (param.type == ParameterType::Object) {
            QJsonValue val = parameters.value(param.name);
            if (!val.isObject()) {
                invalidations.append(QString("Object parameter '%1' is not an object.").arg(param.name));
            }
            else {
                invalidations.append(validateAgainstInfos(val.toObject(), param.subInfos));
            }
        }
        else if (!param.possibleValues.isEmpty()) {
            if (!param.possibleValues.contains(parameters.value(param.name))) {
                invalidations.append(QString("Parameter '%1' has invalid value '%2'.").arg(param.name).arg(parameters.value(param.name).toString()));
            }
        }
        else if (param.type == ParameterType::Integer || param.type == ParameterType::Decimal) {
            if (!param.ranges.isEmpty()) {
                double value = parameters.value(param.name).toDouble();
                bool within = false;
                for (auto range : param.ranges) {
                    if (value >= range.first && value <= range.second) {
                        within = true;
                        break;
                    }
                }
                if (!within) {
                    invalidations.append(QString("Parameter '%1' value '%2' is outside valid range.").arg(param.name).arg(value));
                }
            }
        }
    }

    return invalidations;
}
