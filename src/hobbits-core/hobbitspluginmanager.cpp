#include "hobbitspluginmanager.h"
#include "settingsmanager.h"
#include <QDir>
#include <QPluginLoader>
#include <QSet>

HobbitsPluginManager::HobbitsPluginManager()
{

}

QMap<QString, QObject*> HobbitsPluginManager::loadPluginsFromDirectory(QDir directory, QStringList &warnings)
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

QStringList HobbitsPluginManager::loadPlugins(const QString &pluginPath)
{
    QStringList warnings;

    QMap<QString, QObject*> plugins;
    QSet<QString> blacklist;
    QVariant oldBlacklist = SettingsManager::getPluginLoaderSetting(SettingsManager::PLUGIN_BLACKLIST_KEY);
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
            SettingsManager::setPrivateSetting(SettingsManager::PLUGINS_RUNNING_KEY, QStringList(pluginFile));
            QSharedPointer<AnalyzerInterface> analyzerInstance = QSharedPointer<AnalyzerInterface>(
                    analyzer->createDefaultAnalyzer());
            if (m_analyzers.contains(analyzerInstance->name())) {
                warnings.append(
                        QString("Duplicate Analyzer plugin found with name '%1' - skipping...").arg(
                                analyzerInstance->name()));
            }
            else {
                m_analyzers.insert(analyzerInstance->name(), analyzerInstance);
                m_loadedPluginLocations.insert(analyzerInstance->name(), pluginFile);
            }
            SettingsManager::setPrivateSetting(SettingsManager::PLUGINS_RUNNING_KEY, QStringList());
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
            SettingsManager::setPrivateSetting(SettingsManager::PLUGINS_RUNNING_KEY, QStringList(pluginFile));
            QSharedPointer<OperatorInterface> opInstance =
                QSharedPointer<OperatorInterface>(op->createDefaultOperator());
            if (m_operators.contains(opInstance->name())) {
                warnings.append(
                        QString("Duplicate Operator plugin found with name '%1' - skipping...").arg(
                                opInstance->
                                name()));
            }
            else {
                m_operators.insert(opInstance->name(), opInstance);
                m_loadedPluginLocations.insert(opInstance->name(), pluginFile);
            }
            SettingsManager::setPrivateSetting(SettingsManager::PLUGINS_RUNNING_KEY, QStringList());
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
            SettingsManager::setPrivateSetting(SettingsManager::PLUGINS_RUNNING_KEY, QStringList(pluginFile));
            QSharedPointer<DisplayInterface> displayInstance = QSharedPointer<DisplayInterface>(
                    display->createDefaultDisplay());
            if (m_displays.contains(displayInstance->name())) {
                warnings.append(
                        QString("Duplicate Display plugin found with name '%1' - skipping...").arg(
                                displayInstance->name()));
            }
            else {
                m_displays.insert(displayInstance->name(), displayInstance);
                m_loadedPluginLocations.insert(displayInstance->name(), pluginFile);
            }
            SettingsManager::setPrivateSetting(SettingsManager::PLUGINS_RUNNING_KEY, QStringList());
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
        ImporterExporterInterface *importerExporter = qobject_cast<ImporterExporterInterface*>(plugin);
        if (importerExporter) {
            SettingsManager::setPrivateSetting(SettingsManager::PLUGINS_RUNNING_KEY, QStringList(pluginFile));
            QSharedPointer<ImporterExporterInterface> importerExporterInstance = QSharedPointer<ImporterExporterInterface>(
                    importerExporter->createDefaultImporterExporter());
            if (m_importerExporters.contains(importerExporterInstance->name())) {
                warnings.append(
                        QString("Duplicate Import/Export plugin found with name '%1' - skipping...").arg(
                                importerExporterInstance->name()));
            }
            else {
                m_importerExporters.insert(importerExporterInstance->name(), importerExporterInstance);
                m_loadedPluginLocations.insert(importerExporterInstance->name(), pluginFile);
            }
            SettingsManager::setPrivateSetting(SettingsManager::PLUGINS_RUNNING_KEY, QStringList());
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

QList<QSharedPointer<OperatorInterface>> HobbitsPluginManager::operators() const
{
    return m_operators.values();
}

QList<QSharedPointer<AnalyzerInterface>> HobbitsPluginManager::analyzers() const
{
    return m_analyzers.values();
}

QList<QSharedPointer<DisplayInterface>> HobbitsPluginManager::displays() const
{
    return m_displays.values();
}

QList<QSharedPointer<ImporterExporterInterface>> HobbitsPluginManager::importerExporters() const
{
    return m_importerExporters.values();
}

QSharedPointer<OperatorInterface> HobbitsPluginManager::getOperator(const QString &name) const
{
    return m_operators.value(name, QSharedPointer<OperatorInterface>());
}

QSharedPointer<AnalyzerInterface> HobbitsPluginManager::getAnalyzer(const QString &name) const
{
    return m_analyzers.value(name, QSharedPointer<AnalyzerInterface>());
}

QSharedPointer<DisplayInterface> HobbitsPluginManager::getDisplay(const QString &name) const
{
    return m_displays.value(name, QSharedPointer<DisplayInterface>());
}

QSharedPointer<ImporterExporterInterface> HobbitsPluginManager::getImporterExporter(const QString &name) const
{
    return m_importerExporters.value(name, QSharedPointer<ImporterExporterInterface>());
}

QString HobbitsPluginManager::getPluginLocation(const QString &name) const
{
    return m_loadedPluginLocations.value(name);
}

bool HobbitsPluginManager::addOperator(QString location, QSharedPointer<OperatorInterface> op)
{
    if (m_loadedPluginLocations.contains(op->name())) {
        return false;
    }
    m_operators.insert(op->name(), op);
    m_loadedPluginLocations.insert(op->name(), location);
    return true;
}

bool HobbitsPluginManager::addAnalyzer(QString location, QSharedPointer<AnalyzerInterface> analyzer)
{
    if (m_loadedPluginLocations.contains(analyzer->name())) {
        return false;
    }
    m_analyzers.insert(analyzer->name(), analyzer);
    m_loadedPluginLocations.insert(analyzer->name(), location);
    return true;
}

bool HobbitsPluginManager::addImporterExporter(QString location, QSharedPointer<ImporterExporterInterface> importerExporter)
{
    if (m_loadedPluginLocations.contains(importerExporter->name())) {
        return false;
    }
    m_importerExporters.insert(importerExporter->name(), importerExporter);
    m_loadedPluginLocations.insert(importerExporter->name(), location);
    return true;
}

bool HobbitsPluginManager::addDisplay(QString location, QSharedPointer<DisplayInterface> display)
{
    if (m_loadedPluginLocations.contains(display->name())) {
        return false;
    }
    m_displays.insert(display->name(), display);
    m_loadedPluginLocations.insert(display->name(), location);
    return true;
}
