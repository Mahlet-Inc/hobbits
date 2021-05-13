#ifndef PARAMETERS_H
#define PARAMETERS_H

#include <QJsonObject>

class Parameters
{
public:
    Parameters();
    explicit Parameters(QJsonObject values);
    static Parameters nullParameters();

    bool isNull() const;
    bool contains(const QString &key) const;
    QJsonObject values() const;
    QJsonValue value(const QString &key) const;
    void insert(const QString &key, const QJsonValue &value);

    QJsonValue serialize() const;
    static Parameters deserialize(QJsonValue serialized);

    inline bool operator==(const Parameters &other) const
    {
        if (!m_initialized) {
            return other.isNull();
        }
        else {
            return m_values == other.values();
        }
    }

private:
    bool m_initialized;
    QJsonObject m_values;
};

#endif // PARAMETERS_H
