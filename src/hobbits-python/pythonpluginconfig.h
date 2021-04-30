#ifndef PYTHONPLUGINCONFIG_H
#define PYTHONPLUGINCONFIG_H

#include "parameterdelegate.h"
#include "operatorinterface.h"
#include "hobbitspluginmanager.h"
#include "hobbits-python_global.h"
#include "displayrenderconfig.h"

class HOBBITSPYTHONSHARED_EXPORT PythonPluginConfig
{
public:
    PythonPluginConfig();

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

    static QSharedPointer<ImporterExporterInterface> loadExporter(QString configFolder,
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
    QSharedPointer<DisplayRenderConfig> renderConfig() const;

private:
    QString m_name;
    QString m_description;
    QStringList m_tags;
    QSharedPointer<ParameterDelegate> m_delegate;
    QList<ParameterDelegate::ParameterInfo> m_parameterInfos;
    QString m_script;
    QString m_type;
    QStringList m_extraPaths;
    QSharedPointer<DisplayRenderConfig> m_renderConfig;
};

#endif // PYTHONPLUGINCONFIG_H
