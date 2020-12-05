#ifndef PYTHONPLUGINCONFIG_H
#define PYTHONPLUGINCONFIG_H

#include "parameterdelegate.h"
#include "operatorinterface.h"
#include "hobbitspluginmanager.h"
#include "hobbits-python_global.h"

class HOBBITSPYTHONSHARED_EXPORT PythonPluginConfig
{
public:
    PythonPluginConfig();

    enum NumberType {
        Integer = 1,
        Decimal = 2
    };

    static QStringList loadPythonPlugins(QString path,
                                         QSharedPointer<HobbitsPluginManager> pluginManager,
                                         std::function<AbstractParameterEditor *(QSharedPointer<ParameterDelegate>, QSize)> editorCreator);

    static QSharedPointer<OperatorInterface> loadOperator(QString configFolder,
                                                          std::function<AbstractParameterEditor *(QSharedPointer<ParameterDelegate>, QSize)> editorCreator,
                                                          QStringList &errors);

    static QSharedPointer<AnalyzerInterface> loadAnalyzer(QString configFolder,
                                                          std::function<AbstractParameterEditor *(QSharedPointer<ParameterDelegate>, QSize)> editorCreator,
                                                          QStringList &errors);

    static QSharedPointer<ImporterExporterInterface> loadImporter(QString configFolder,
                                                          std::function<AbstractParameterEditor *(QSharedPointer<ParameterDelegate>, QSize)> editorCreator,
                                                          QStringList &errors);

    static QSharedPointer<DisplayInterface> loadDisplay(QString configFolder,
                                                          std::function<AbstractParameterEditor *(QSharedPointer<ParameterDelegate>, QSize)> editorCreator,
                                                          QStringList &errors);

    QStringList configure(QString configFolder, std::function<AbstractParameterEditor *(QSharedPointer<ParameterDelegate>, QSize)> editorCreator);

    QString name() const;
    QString description() const;
    QStringList tags() const;
    QSharedPointer<ParameterDelegate> delegate() const;
    QList<ParameterDelegate::ParameterInfo> parameterInfos() const;
    QString script() const;
    QString type() const;
    QStringList extraPaths() const;
    NumberType parameterNumberType(QString paramName);

private:
    QString m_name;
    QString m_description;
    QStringList m_tags;
    QSharedPointer<ParameterDelegate> m_delegate;
    QList<ParameterDelegate::ParameterInfo> m_parameterInfos;
    QMap<QString, NumberType> m_paramNumberTypes;
    QString m_script;
    QString m_type;
    QStringList m_extraPaths;
};

#endif // PYTHONPLUGINCONFIG_H
