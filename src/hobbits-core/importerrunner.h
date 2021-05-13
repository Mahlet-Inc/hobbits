#ifndef IMPORTERRUNNER_H
#define IMPORTERRUNNER_H

#include "importexportinterface.h"
#include "abstractpluginrunner.h"
#include "hobbitspluginmanager.h"
#include "bitcontainermanager.h"

/**
  * @brief The ImporterRunner class manages the execution of an ImporterExporterInterface PluginAction
  *
  * \see ImporterExporterInterface PluginAction
*/
class HOBBITSCORESHARED_EXPORT ImporterRunner : public AbstractPluginRunner<ImportResult>
{
    Q_OBJECT

public:
    static QSharedPointer<ImporterRunner> create(
            QSharedPointer<const HobbitsPluginManager> pluginManager,
            QSharedPointer<BitContainerManager> containerManager,
            QSharedPointer<const PluginAction> action);

    QSharedPointer<PluginActionWatcher<QSharedPointer<ImportResult>>> run();

private slots:
    void postProcess() override;

private:
    ImporterRunner(QString pluginName, QString pluginFileLocation);
    static QSharedPointer<ImportResult> importerCall(
            QSharedPointer<ImporterExporterInterface> importer,
            const Parameters &parameters,
            QSharedPointer<PluginActionProgress> progressTracker);

    QSharedPointer<const PluginAction> m_action;
    QSharedPointer<ImporterExporterInterface> m_importer;
    QSharedPointer<BitContainerManager> m_containerManager;
};
#endif // IMPORTERRUNNER_H
