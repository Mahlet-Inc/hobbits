#ifndef EXPORTERRUNNER_H
#define EXPORTERRUNNER_H

#include "importexportinterface.h"
#include "abstractpluginrunner.h"
#include "hobbitspluginmanager.h"
#include "bitcontainer.h"

/**
  * @brief The ExporterRunner class manages the execution of an ImporterExporterInterface PluginAction
  *
  * \see ImporterExporterInterface PluginAction
*/
class HOBBITSCORESHARED_EXPORT ExporterRunner: public AbstractPluginRunner<ExportResult>
{
    Q_OBJECT

public:
    static QSharedPointer<ExporterRunner> create(
            QSharedPointer<const HobbitsPluginManager> pluginManager,
            QSharedPointer<const PluginAction> action);

    QSharedPointer<PluginActionWatcher<QSharedPointer<ExportResult>>> run(QSharedPointer<BitContainer> container);

private slots:
    void postProcess() override;

private:
    ExporterRunner(QString pluginName, QString pluginFileLocation);

    static QSharedPointer<ExportResult> exporterCall(
            QSharedPointer<ImporterExporterInterface> exporter,
            QSharedPointer<const BitContainer> container,
            const Parameters &parameters,
            QSharedPointer<PluginActionProgress> progressTracker);

    QSharedPointer<const PluginAction> m_action;
    QSharedPointer<ImporterExporterInterface> m_exporter;
};

#endif // EXPORTERRUNNER_H
