#ifndef OPERATORACTOR_H
#define OPERATORACTOR_H

#include "pluginactionwatcher.h"
#include "bitcontainermanager.h"
#include "hobbitspluginmanager.h"
#include "parameters.h"
#include <QMap>
#include <QObject>
#include <QtConcurrent/QtConcurrentRun>
#include "bitcontainermanager.h"
#include "hobbits-core_global.h"
#include "abstractpluginrunner.h"

/**
  * @brief The OperatorRunner class manages the execution of an OperatorInterface PluginAction
  *
  * \see OperatorInterface PluginAction
*/
class HOBBITSCORESHARED_EXPORT OperatorRunner : public AbstractPluginRunner<const OperatorResult>
{
    Q_OBJECT

public:
    static QSharedPointer<OperatorRunner> create(
            QSharedPointer<const HobbitsPluginManager> pluginManager,
            QSharedPointer<BitContainerManager> containerManager,
            QSharedPointer<const PluginAction> action);

    QSharedPointer<PluginActionWatcher<QSharedPointer<const OperatorResult>>> run(QList<QSharedPointer<BitContainer>> inputContainers);

private slots:
    void postProcess() override;

private:
    OperatorRunner(QString pluginName, QString pluginFileLocation);
    static QSharedPointer<const OperatorResult> operatorCall(
            QSharedPointer<OperatorInterface> op,
            QList<QSharedPointer<const BitContainer>> inputContainers,
            const Parameters &parameters,
            QSharedPointer<PluginActionProgress> progressTracker);

    QSharedPointer<const PluginAction> m_action;
    QSharedPointer<OperatorInterface> m_op;
    QList<QSharedPointer<BitContainer>> m_inputContainers;

    QSharedPointer<BitContainerManager> m_containerManager;
};

#endif // OPERATORACTOR_H
