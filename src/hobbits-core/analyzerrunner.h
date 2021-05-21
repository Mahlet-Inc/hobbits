#ifndef ANALYZERRUNNER_H
#define ANALYZERRUNNER_H

#include "pluginactionwatcher.h"
#include "analyzerresult.h"
#include "hobbitspluginmanager.h"
#include "parameters.h"
#include <QObject>
#include <QtConcurrent/QtConcurrentRun>
#include "hobbits-core_global.h"
#include "abstractpluginrunner.h"

/**
  * @brief The AnalyzerRunner class manages the execution of an AnalyzerInterface PluginAction
  *
  * \see AnalyzerInterface PluginAction
*/
class HOBBITSCORESHARED_EXPORT AnalyzerRunner : public AbstractPluginRunner<const AnalyzerResult>
{
    Q_OBJECT

public:
    static QSharedPointer<AnalyzerRunner> create(
            QSharedPointer<const HobbitsPluginManager> pluginManager,
            QSharedPointer<const PluginAction> action);

    QSharedPointer<PluginActionWatcher<QSharedPointer<const AnalyzerResult>>> run(QSharedPointer<BitContainer> container);

    QSharedPointer<BitContainer> container() const;

private slots:
    void postProcess() override;

private:
    AnalyzerRunner(QString pluginName, QString pluginFileLocation);
    static QSharedPointer<const AnalyzerResult> analyzerCall(
            QSharedPointer<AnalyzerInterface> analyzer,
            QSharedPointer<const BitContainer> bits,
            const Parameters &parameters,
            QSharedPointer<PluginActionProgress> progressTracker);

    QSharedPointer<const PluginAction> m_action;
    QSharedPointer<AnalyzerInterface> m_analyzer;
    QSharedPointer<BitContainer> m_container;
};

#endif // ANALYZERRUNNER_H
