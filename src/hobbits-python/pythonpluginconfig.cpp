#include "pythonpluginconfig.h"
#include <QDir>
#include "pythonoperator.h"
#include "pythonimporter.h"
#include "pythonexporter.h"
#include "pythonanalyzer.h"
#include "pythondisplay.h"
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

    QDir exporterDir(path + "/python_exporters");
    for (QString subDir : exporterDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        QSharedPointer<PythonPluginConfig> config(new PythonPluginConfig());
        auto pluginDir = exporterDir.absoluteFilePath(subDir);
        auto plugin = loadExporter(pluginDir, editorCreator, errors);
        if (plugin.isNull()) {
            continue;
        }
        if (!pluginManager->addImporterExporter(pluginDir, plugin)) {
            errors.append(QString("Duplicate plugin %1 not loaded from %2").arg(plugin->name()).arg(pluginDir));
        }
    }

    QDir displayDir(path + "/python_displays");
    for (QString subDir : displayDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        QSharedPointer<PythonPluginConfig> config(new PythonPluginConfig());
        auto pluginDir = displayDir.absoluteFilePath(subDir);
        auto plugin = loadDisplay(pluginDir, editorCreator, errors);
        if (plugin.isNull()) {
            continue;
        }
        if (!pluginManager->addDisplay(pluginDir, plugin)) {
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

QSharedPointer<ImporterExporterInterface> PythonPluginConfig::loadExporter(QString configFolder, std::function<AbstractParameterEditor *(QSharedPointer<ParameterDelegate>, QSize)> editorCreator, QStringList &errors)
{
    QSharedPointer<PythonPluginConfig> config(new PythonPluginConfig());
    auto configErrors = config->configure(configFolder, editorCreator);
    if (!configErrors.isEmpty()) {
        errors.append(configErrors);
        return nullptr;
    }
    if (config->type() != "exporter") {
        return nullptr;
    }
    return QSharedPointer<ImporterExporterInterface>(new PythonExporter(config));
}

QSharedPointer<DisplayInterface> PythonPluginConfig::loadDisplay(QString configFolder, std::function<AbstractParameterEditor *(QSharedPointer<ParameterDelegate>, QSize)> editorCreator, QStringList &errors)
{
    QSharedPointer<PythonPluginConfig> config(new PythonPluginConfig());
    auto configErrors = config->configure(configFolder, editorCreator);
    if (!configErrors.isEmpty()) {
        errors.append(configErrors);
        return nullptr;
    }
    if (config->type() != "display") {
        return nullptr;
    }

    return QSharedPointer<DisplayInterface>(new PythonDisplay(config));
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
            ParameterDelegate::ParameterInfo info = {name, QJsonValue::Double};
            if (param.contains("min") || param.contains("max")) {
                info.hasIntLimits = true;
                info.intMax = INT_MAX;
                info.intMin = INT_MIN;
            }
            if (param.contains("min")) {
                info.intMin = param.value("min").toInt();
            }
            if (param.contains("max")) {
                info.intMax = param.value("max").toInt();
            }
            m_parameterInfos.append(info);
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

    if (m_type == "display") {
        m_renderConfig = QSharedPointer<DisplayRenderConfig>(new DisplayRenderConfig());
        m_renderConfig->setAsynchronous(true);
        m_renderConfig->setFullRedrawTriggers(DisplayRenderConfig::NewFrameOffset | DisplayRenderConfig::NewBitOffset);

        if (config.contains("render_config")) {
            if (!config.value("render_config").isObject()) {
                return {"render_config field is formatted incorrectly"};
            }
            QJsonObject rConfig = config.value("render_config").toObject();
            if (rConfig.contains("asynchronous")) {
                m_renderConfig->setAsynchronous(rConfig.value("asynchronous").toBool());
            }
            if (rConfig.contains("hide_frame_offset_controls")) {
                m_renderConfig->setHideFrameOffsetControls(rConfig.value("hide_frame_offset_controls").toBool());
            }
            if (rConfig.contains("hide_bit_offset_controls")) {
                m_renderConfig->setHideBitOffsetControls(rConfig.value("hide_bit_offset_controls").toBool());
            }
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

QSharedPointer<DisplayRenderConfig> PythonPluginConfig::renderConfig() const
{
    return m_renderConfig;
}

PythonPluginConfig::NumberType PythonPluginConfig::parameterNumberType(QString paramName)
{
    return m_paramNumberTypes.value(paramName, NumberType::Decimal);
}
