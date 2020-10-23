#include "pythonpluginconfig.h"
#include <QDir>
#include "pythonoperator.h"
#include "pythonimporter.h"
#include "pythonanalyzer.h"
#include <QJsonArray>

PythonPluginConfig::PythonPluginConfig()
{

}

QStringList PythonPluginConfig::loadPythonPlugins(QString path,
                                                  QSharedPointer<HobbitsPluginManager> pluginManager,
                                                  std::function<AbstractParameterEditor *(QSharedPointer<ParameterDelegate>, QSize)> editorCreator)
{
    QStringList errors;
    QDir operatorDir(path + "/python_operators");
    for (QString subDir : operatorDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        QSharedPointer<PythonPluginConfig> config(new PythonPluginConfig());
        auto pluginDir = operatorDir.absoluteFilePath(subDir);
        auto plugin = loadOperator(pluginDir, editorCreator, errors);
        if (plugin.isNull()) {
            continue;
        }
        if (!pluginManager->addOperator(pluginDir, plugin)) {
            errors.append(QString("Duplicate plugin %1 not loaded from %2").arg(plugin->name()).arg(pluginDir));
        }
    }

    QDir analyzerDir(path + "/python_analyzers");
    for (QString subDir : analyzerDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        QSharedPointer<PythonPluginConfig> config(new PythonPluginConfig());
        auto pluginDir = analyzerDir.absoluteFilePath(subDir);
        auto plugin = loadAnalyzer(pluginDir, editorCreator, errors);
        if (plugin.isNull()) {
            continue;
        }
        if (!pluginManager->addAnalyzer(pluginDir, plugin)) {
            errors.append(QString("Duplicate plugin %1 not loaded from %2").arg(plugin->name()).arg(pluginDir));
        }
    }

    QDir importerDir(path + "/python_importers");
    for (QString subDir : importerDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        QSharedPointer<PythonPluginConfig> config(new PythonPluginConfig());
        auto pluginDir = importerDir.absoluteFilePath(subDir);
        auto plugin = loadImporter(pluginDir, editorCreator, errors);
        if (plugin.isNull()) {
            continue;
        }
        if (!pluginManager->addImporterExporter(pluginDir, plugin)) {
            errors.append(QString("Duplicate plugin %1 not loaded from %2").arg(plugin->name()).arg(pluginDir));
        }
    }
    return errors;
}

QSharedPointer<OperatorInterface> PythonPluginConfig::loadOperator(QString configFolder,
                                                                   std::function<AbstractParameterEditor *(QSharedPointer<ParameterDelegate>, QSize)> editorCreator,
                                                                   QStringList &errors)
{
    QSharedPointer<PythonPluginConfig> config(new PythonPluginConfig());
    auto configErrors = config->configure(configFolder, editorCreator);
    if (!configErrors.isEmpty()) {
        errors.append(configErrors);
        return nullptr;
    }
    if (config->type() != "operator") {
        return nullptr;
    }
    return QSharedPointer<OperatorInterface>(new PythonOperator(config));
}

QSharedPointer<AnalyzerInterface> PythonPluginConfig::loadAnalyzer(QString configFolder,
                                                                   std::function<AbstractParameterEditor *(QSharedPointer<ParameterDelegate>, QSize)> editorCreator,
                                                                   QStringList &errors)
{
    QSharedPointer<PythonPluginConfig> config(new PythonPluginConfig());
    auto configErrors = config->configure(configFolder, editorCreator);
    if (!configErrors.isEmpty()) {
        errors.append(configErrors);
        return nullptr;
    }
    if (config->type() != "analyzer") {
        return nullptr;
    }
    return QSharedPointer<AnalyzerInterface>(new PythonAnalyzer(config));
}

QSharedPointer<ImporterExporterInterface> PythonPluginConfig::loadImporter(QString configFolder, std::function<AbstractParameterEditor *(QSharedPointer<ParameterDelegate>, QSize)> editorCreator, QStringList &errors)
{
    QSharedPointer<PythonPluginConfig> config(new PythonPluginConfig());
    auto configErrors = config->configure(configFolder, editorCreator);
    if (!configErrors.isEmpty()) {
        errors.append(configErrors);
        return nullptr;
    }
    if (config->type() != "importer") {
        return nullptr;
    }
    return QSharedPointer<ImporterExporterInterface>(new PythonImporter(config));
}

QStringList PythonPluginConfig::configure(QString configFolder,
                                          std::function<AbstractParameterEditor *(QSharedPointer<ParameterDelegate>, QSize)> editorCreator)
{
    QDir dir(configFolder);

    QFile configFile(dir.filePath("plugin.json"));
    if (!configFile.exists()) {
        return {"No plugin.json file was not found"};
    }
    if (!configFile.open(QFile::ReadOnly)) {
        return {"The plugin.json file could not be opened"};
    }

    QJsonObject config = QJsonDocument::fromJson(configFile.readAll()).object();
    if (!config.contains("name")) {
        return {"Missing 'name' field in plugin.json config"};
    }
    m_name = config.value("name").toString();

    if (!config.contains("description")) {
        return {"Missing 'description' field in plugin.json config"};
    }
    m_description = config.value("description").toString();

    if (!config.contains("tags")) {
        return {"Missing 'description' field in plugin.json config"};
    }
    for (auto value : config.value("tags").toArray()) {
        m_tags.append(value.toString());
    }

    if (!config.contains("parameters")) {
        return {"Missing 'parameters' field in plugin.json config"};
    }
    for (auto value : config.value("parameters").toArray()) {
        auto param = value.toObject();
        QString name = param.value("name").toString();
        QString type = param.value("type").toString();
        if (type == "string") {
            m_parameterInfos.append({name, QJsonValue::String});
        }
        else if (type == "integer") {
            m_parameterInfos.append({name, QJsonValue::Double});
            m_paramNumberTypes.insert(name, NumberType::Integer);
        }
        else if (type == "decimal") {
            m_parameterInfos.append({name, QJsonValue::Double});
            m_paramNumberTypes.insert(name, NumberType::Decimal);
        }
        else if (type == "boolean") {
            m_parameterInfos.append({name, QJsonValue::Bool});
        }
    }

    if (!config.contains("script")) {
        return {"Missing 'script' field in plugin.json config"};
    }
    QFile scriptFile(dir.filePath(config.value("script").toString()));
    if (!scriptFile.exists()) {
        return {"The specified script file was not found"};
    }
    if (!scriptFile.open(QFile::ReadOnly)) {
        return {"The script file could not be opened"};
    }
    m_script = scriptFile.readAll();

    m_delegate = ParameterDelegate::create(m_parameterInfos,
                                           [this](QJsonObject parameters) {
                                                       Q_UNUSED(parameters)
                                                       return QString("Run %1").arg(m_name);
                                           }, editorCreator);

    if (!config.contains("type")) {
        return {"Missing 'type' field in plugin.json config"};
    }
    m_type = config.value("type").toString();

    if (config.contains("extra_paths")) {
        for (auto path : config.value("extra_paths").toArray()) {
            m_extraPaths.append(path.toString());
        }
    }

    return {};
}

QString PythonPluginConfig::name() const
{
    return m_name;
}

QString PythonPluginConfig::description() const
{
    return m_description;
}

QStringList PythonPluginConfig::tags() const
{
    return m_tags;
}

QSharedPointer<ParameterDelegate> PythonPluginConfig::delegate() const
{
    return m_delegate;
}

QList<ParameterDelegate::ParameterInfo> PythonPluginConfig::parameterInfos() const
{
    return m_parameterInfos;
}

QString PythonPluginConfig::script() const
{
    return m_script;
}

QString PythonPluginConfig::type() const
{
    return m_type;
}

QStringList PythonPluginConfig::extraPaths() const
{
    return m_extraPaths;
}

PythonPluginConfig::NumberType PythonPluginConfig::parameterNumberType(QString paramName)
{
    return m_paramNumberTypes.value(paramName, NumberType::Decimal);
}
