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

class HOBBITSCORESHARED_EXPORT AbstractParameterEditor;

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
    struct HOBBITSCORESHARED_EXPORT ParameterInfo
    {
        QString name;
        QJsonValue::Type type;
        bool optional;
        QList<ParameterInfo> subInfos;

        ParameterInfo(QString name, QJsonValue::Type type, bool optional = true, QList<ParameterInfo> subInfos = {}):
            name{name}, type{type}, optional{optional}, subInfos{subInfos} {}

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

    virtual AbstractParameterEditor* createEditor(QSize targetBounds = QSize());

    QList<ParameterInfo> parameterInfos() const;
    ParameterInfo getInfo(QString name) const;

    bool validate(const QJsonObject &parameters) const;
    QString actionDescription(const QJsonObject &parameters) const;

protected:
    static bool validateAgainstInfos(const QJsonObject &parameters, QList<ParameterInfo> infos);
    QMap<QString, ParameterInfo> m_parameterMap;
    std::function<QString(const QJsonObject&)> m_actionDescriber;
    std::function<AbstractParameterEditor*(QSharedPointer<ParameterDelegate>, QSize)> m_editorCreator;
};

#endif // PLUGINSTATEDELEGATE_H
