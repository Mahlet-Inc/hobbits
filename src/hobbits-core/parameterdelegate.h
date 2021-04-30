#ifndef PLUGINSTATEDELEGATE_H
#define PLUGINSTATEDELEGATE_H

#include "hobbits-core_global.h"
#include <QObject>
#include <QJsonValue>
#include <QSize>
#include <QMap>
#include <QVector>
#include <QSharedPointer>
#include <QJsonObject>
#include "hobbits-core_global.h"
#include "fwd_abstractparametereditor.h"

/**
  * @brief The ParameterDelegate class facilitates plugin parameter validation and configuration
  *
  * A plugin that accepts parameters must provide a ParameterDelegate that contains
  * specifications for valid plugin parameters. The ParameterDelegate must also act as a
  * ParameterEditorInterface factory if it wants interactive applications to be capable of
  * configuring the plugin.
  *
  * The AnalyzerInterface, OperatorInterface, and ImportExportInterface classes are all
  * expected to provide a ParameterDelegate.
  *
*/
class HOBBITSCORESHARED_EXPORT ParameterDelegate : public QEnableSharedFromThis<ParameterDelegate>
{
public:

    /**
     * @brief The ParameterType enum provides type classifications for parameters
     *
     * ParameterType is similar to QJsonValue, but without null or undefined, and with a
     * decimal/integer distinction.
     */
    enum ParameterType
    {
        Boolean = 0x1,
        Decimal = 0x2,
        String = 0x3,
        Array = 0x4,
        Object = 0x5,
        Integer = 0x10
    };

    /**
     * @brief The ParameterInfo struct contains information for a parameter
     *
     * A parameter's ParameterInfo can be used to provide editors and validation for a parameter.
     */
    struct HOBBITSCORESHARED_EXPORT ParameterInfo
    {
        QString name;
        ParameterType type;
        bool optional;
        QList<ParameterInfo> subInfos;

        QList<QPair<double, double>> ranges;
        QList<QJsonValue> possibleValues;

        ParameterInfo(QString name, ParameterType type, bool optional = true, QList<ParameterInfo> subInfos = {}):
            name{name}, type{type}, optional{optional}, subInfos{subInfos}, ranges(QList<QPair<double, double>>()), possibleValues(QList<QJsonValue>()) {}

        ParameterInfo() = default;
        ParameterInfo(const ParameterInfo&) = default;
    };

    ParameterDelegate(QList<ParameterInfo> parameterInfos, std::function<QString(const QJsonObject&)> actionDescriber);
    ParameterDelegate(QList<ParameterInfo> parameterInfos,
                      std::function<QString(const QJsonObject&)> actionDescriber,
                      std::function<AbstractParameterEditor*(QSharedPointer<ParameterDelegate>, QSize)> editorCreator);
    virtual ~ParameterDelegate() = default;

    static QSharedPointer<ParameterDelegate> create(
            QList<ParameterInfo> parameterInfos,
            std::function<QString(const QJsonObject&)> actionDescriber,
            std::function<AbstractParameterEditor*(QSharedPointer<ParameterDelegate>, QSize)> editorCreator);

    static bool jsonTypeCompatible(QJsonValue::Type jsonType, ParameterType type);

    virtual AbstractParameterEditor* createEditor(QSize targetBounds = QSize());

    QList<ParameterInfo> parameterInfos() const;
    ParameterInfo getInfo(QString name) const;

    QStringList validate(const QJsonObject &parameters) const;
    QString actionDescription(const QJsonObject &parameters) const;

protected:
    static QStringList validateAgainstInfos(const QJsonObject &parameters, QList<ParameterInfo> infos);
    QMap<QString, ParameterInfo> m_parameterMap;
    std::function<QString(const QJsonObject&)> m_actionDescriber;
    std::function<AbstractParameterEditor*(QSharedPointer<ParameterDelegate>, QSize)> m_editorCreator;
};

#endif // PLUGINSTATEDELEGATE_H
