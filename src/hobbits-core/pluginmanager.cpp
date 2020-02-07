#include "pluginmanager.h"
#include "settingsmanager.h"
#include <QDir>
#include <QPluginLoader>

PluginManager::PluginManager()
{

}

QMap<QString, QObject*> PluginManager::loadPluginsFromDirectory(QDir directory, QStringList &warnings)
{
    QMap<QString, QObject*> plugins;

    QList<QDir> pluginDirs;
    pluginDirs.append(directory);
    // Check subdirs, 1 deep
    for (QString subDir : directory.entryList(QDir::Dirs | QDir::NoDotAndDotDot)) {
        pluginDirs.append(directory.absoluteFilePath(subDir));
    }

    for (QDir pluginDir : pluginDirs) {
        for (QString dfileName : pluginDir.entryList(QDir::Files)) {
            if (!(dfileName.endsWith(".so")
                  || dfileName.endsWith(".dll")
                  || dfileName.endsWith(".dylib"))) {
                continue;
            }
            QString fileName = pluginDir.absoluteFilePath(dfileName);
            QPluginLoader loader(fileName);
            QObject *plugin = loader.instance();
            if (plugin) {
                plugins.insert(fileName, plugin);
            }
            else {
                warnings.append(QString("File '%1' could not be loaded as a plugin - skipping...").arg(fileName));
            }
        }
    }

    return plugins;
}

QStringList PluginManager::loadPlugins(const QString &pluginPath)
{
    QStringList warnings;

    QMap<QString, QObject*> plugins;
    QSet<QString> blacklist;
    QVariant oldBlacklist = SettingsManager::getInstance().getPluginLoaderSetting(SettingsData::PLUGIN_BLACKLIST_KEY);
    if (oldBlacklist.isValid() && oldBlacklist.canConvert<QStringList>()) {
        blacklist = oldBlacklist.toStringList().toSet();
    }

    // Load analyzer plugins
    QDir analyzersDir(pluginPath + "/analyzers");
    plugins = loadPluginsFromDirectory(analyzersDir, warnings);
    for (QString pluginFile : plugins.keys()) {
        if (blacklist.contains(pluginFile)) {
            continue;
        }
        QObject *plugin = plugins.value(pluginFile);
        AnalyzerInterface *analyzer = qobject_cast<AnalyzerInterface*>(plugin);
        if (analyzer) {
            SettingsManager::getInstance().setPrivateSetting(SettingsData::PLUGIN_RUNNING_KEY, QVariant(pluginFile));
            QSharedPointer<AnalyzerInterface> analyzerInstance = QSharedPointer<AnalyzerInterface>(
                    analyzer->createDefaultAnalyzer());
            if (m_analyzers.contains(analyzerInstance->getName())) {
                warnings.append(
                        QString("Duplicate Analyzer plugin found with name '%1' - skipping...").arg(
                                analyzerInstance->getName()));
            }
            else {
                m_analyzers.insert(analyzerInstance->getName(), analyzerInstance);
                m_loadedPluginLocations.insert(analyzerInstance->getName(), pluginFile);
            }
            SettingsManager::getInstance().setPrivateSetting(SettingsData::PLUGIN_RUNNING_KEY, QVariant());
        }
        else {
            warnings.append(
                    QString("Plugin loaded from '%1' was not a valid Analyzer implementation - skipping...").arg(
                            pluginFile));
        }
    }

    // Load Operator plugins
    QDir operatorsDir(pluginPath + "/operators");
    plugins = loadPluginsFromDirectory(operatorsDir, warnings);
    for (QString pluginFile : plugins.keys()) {
        if (blacklist.contains(pluginFile)) {
            continue;
        }
        QObject *plugin = plugins.value(pluginFile);
        OperatorInterface *op = qobject_cast<OperatorInterface*>(plugin);
        if (op) {
            SettingsManager::getInstance().setPrivateSetting(SettingsData::PLUGIN_RUNNING_KEY, QVariant(pluginFile));
            QSharedPointer<OperatorInterface> opInstance =
                QSharedPointer<OperatorInterface>(op->createDefaultOperator());
            if (m_operators.contains(opInstance->getName())) {
                warnings.append(
                        QString("Duplicate Operator plugin found with name '%1' - skipping...").arg(
                                opInstance->
                                getName()));
            }
            else {
                m_operators.insert(opInstance->getName(), opInstance);
                m_loadedPluginLocations.insert(opInstance->getName(), pluginFile);
            }
            SettingsManager::getInstance().setPrivateSetting(SettingsData::PLUGIN_RUNNING_KEY, QVariant());
        }
        else {
            warnings.append(
                    QString("Plugin loaded from '%1' was not a valid Operator implementation - skipping...").arg(
                            pluginFile));
        }
    }

    // Load Display plugins
    QDir displaysDir(pluginPath + "/displays");
    plugins = loadPluginsFromDirectory(displaysDir, warnings);
    for (QString pluginFile : plugins.keys()) {
        if (blacklist.contains(pluginFile)) {
            continue;
        }
        QObject *plugin = plugins.value(pluginFile);
        DisplayInterface *display = qobject_cast<DisplayInterface*>(plugin);
        if (display) {
            SettingsManager::getInstance().setPrivateSetting(SettingsData::PLUGIN_RUNNING_KEY, QVariant(pluginFile));
            QSharedPointer<DisplayInterface> displayInstance = QSharedPointer<DisplayInterface>(
                    display->createDefaultDisplay());
            if (m_displays.contains(displayInstance->getName())) {
                warnings.append(
                        QString("Duplicate Display plugin found with name '%1' - skipping...").arg(
                                displayInstance->getName()));
            }
            else {
                m_displays.insert(displayInstance->getName(), displayInstance);
                m_loadedPluginLocations.insert(displayInstance->getName(), pluginFile);
            }
            SettingsManager::getInstance().setPrivateSetting(SettingsData::PLUGIN_RUNNING_KEY, QVariant());
        }
        else {
            warnings.append(
                    QString("Plugin loaded from '%1' was not a valid Display implementation - skipping...").arg(
                            pluginFile));
        }
    }

    // Load Import/Export plugins
    QDir importExportDir(pluginPath + "/importerexporters");
    plugins = loadPluginsFromDirectory(importExportDir, warnings);
    for (QString pluginFile : plugins.keys()) {
        if (blacklist.contains(pluginFile)) {
            continue;
        }
        QObject *plugin = plugins.value(pluginFile);
        ImportExportInterface *importerExporter = qobject_cast<ImportExportInterface*>(plugin);
        if (importerExporter) {
            SettingsManager::getInstance().setPrivateSetting(SettingsData::PLUGIN_RUNNING_KEY, QVariant(pluginFile));
            QSharedPointer<ImportExportInterface> importerExporterInstance = QSharedPointer<ImportExportInterface>(
                    importerExporter->createDefaultImporterExporter());
            if (m_displays.contains(importerExporterInstance->getName())) {
                warnings.append(
                        QString("Duplicate Import/Export plugin found with name '%1' - skipping...").arg(
                                importerExporterInstance->getName()));
            }
            else {
                m_importerExporters.insert(importerExporterInstance->getName(), importerExporterInstance);
                m_loadedPluginLocations.insert(importerExporterInstance->getName(), pluginFile);
            }
            SettingsManager::getInstance().setPrivateSetting(SettingsData::PLUGIN_RUNNING_KEY, QVariant());
        }
        else {
            warnings.append(
                    QString(
                            "Plugin loaded from '%1' was not a valid Import/Export implementation - skipping...").arg(
                            pluginFile));
        }
    }

    return warnings;
}

QList<QSharedPointer<OperatorInterface>> PluginManager::getAllOperators() const
{
    return m_operators.values();
}

QList<QSharedPointer<AnalyzerInterface>> PluginManager::getAllAnalyzers() const
{
    return m_analyzers.values();
}

QList<QSharedPointer<DisplayInterface>> PluginManager::getAllDisplays() const
{
    return m_displays.values();
}

QList<QSharedPointer<ImportExportInterface>> PluginManager::getAllImporterExporters() const
{
    return m_importerExporters.values();
}

QSharedPointer<OperatorInterface> PluginManager::getOperator(const QString &name) const
{
    return m_operators.value(name, QSharedPointer<OperatorInterface>());
}

QSharedPointer<AnalyzerInterface> PluginManager::getAnalyzer(const QString &name) const
{
    return m_analyzers.value(name, QSharedPointer<AnalyzerInterface>());
}

QSharedPointer<DisplayInterface> PluginManager::getDisplay(const QString &name) const
{
    return m_displays.value(name, QSharedPointer<DisplayInterface>());
}

QSharedPointer<ImportExportInterface> PluginManager::getImporterExporter(const QString &name) const
{
    return m_importerExporters.value(name, QSharedPointer<ImportExportInterface>());
}

QString PluginManager::getPluginLocation(const QString &name) const
{
    return m_loadedPluginLocations.value(name);
}
