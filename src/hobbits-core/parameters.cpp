#include "parameters.h"


Parameters::Parameters() :
    m_initialized(true),
    m_values(QJsonObject()) {

}

Parameters::Parameters(QJsonObject values) :
    m_initialized(true),
    m_values(values) {

}

Parameters Parameters::nullParameters() {
    Parameters p;
    p.m_initialized = false;
    return p;
}

bool Parameters::isNull() const {
    return !m_initialized;
}

bool Parameters::contains(const QString &key) const {
    return m_values.contains(key);
}

QJsonObject Parameters::values() const {
    return m_values;
}

QJsonValue Parameters::value(const QString &key) const {
    if (!m_values.contains(key)) {
        return QJsonValue();
    }
    return m_values.value(key);
}

void Parameters::insert(const QString &key, const QJsonValue &value) {
    m_initialized = true;
    m_values.insert(key, value);
}

QJsonValue Parameters::serialize() const {
    if (!m_initialized) {
        return QJsonValue();
    }
    return values();
}

Parameters Parameters::deserialize(QJsonValue serialized) {
    if (serialized.isObject()) {
        return Parameters(serialized.toObject());
    }
    return Parameters();
}
