#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include <QDir>
#include <QMap>
#include <QString>

#include "analyzerinterface.h"
#include "displayinterface.h"
#include "importexportinterface.h"
#include "operatorinterface.h"

#include "hobbits-core_global.h"

class HOBBITSCORESHARED_EXPORT PluginManager
{
public:
    PluginManager();

    // Returns a list of warnings
    QStringList loadPlugins(const QString &pluginPath);

    QList<QSharedPointer<OperatorInterface>> getAllOperators() const;
    QList<QSharedPointer<AnalyzerInterface>> getAllAnalyzers() const;
    QList<QSharedPointer<DisplayInterface>> getAllDisplays() const;
    QList<QSharedPointer<ImportExportInterface>> getAllImporterExporters() const;

    QSharedPointer<OperatorInterface> getOperator(const QString &name) const;
    QSharedPointer<AnalyzerInterface> getAnalyzer(const QString &name) const;
    QSharedPointer<DisplayInterface> getDisplay(const QString &name) const;
    QSharedPointer<ImportExportInterface> getImporterExporter(const QString &name) const;

    QString getPluginLocation(const QString &name) const;

private:
    QMap<QString, QSharedPointer<OperatorInterface>> m_operators;
    QMap<QString, QSharedPointer<AnalyzerInterface>> m_analyzers;
    QMap<QString, QSharedPointer<DisplayInterface>> m_displays;
    QMap<QString, QSharedPointer<ImportExportInterface>> m_importerExporters;

    QMap<QString, QString> m_loadedPluginLocations;

    QMap<QString, QObject*> loadPluginsFromDirectory(QDir directory, QStringList &warnings);

};

#endif // PLUGINMANAGER_H
