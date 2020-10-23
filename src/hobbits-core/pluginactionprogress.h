#ifndef ACTIONPROGRESS_H
#define ACTIONPROGRESS_H

#include <QMutex>
#include <QObject>
#include "hobbits-core_global.h"

/**
  * @brief The PluginActionProgress class facilitates communication to and from executing plugin actions
  *
  * \see PluginAction PluginActionWatcher
*/
class HOBBITSCORESHARED_EXPORT PluginActionProgress : public QObject
{
    Q_OBJECT

public:
    PluginActionProgress(QObject *parent = nullptr);

    bool isCancelled();

public Q_SLOTS:
    void setProgressPercent(int progressPercent);
    void setCancelled(bool cancelled);

    void setProgress(qint64 completed, qint64 required);
    void setProgress(int completed, int required);
    void setProgress(double completed, double required);

Q_SIGNALS:
    void progressPercentChanged(int);

protected:
    int m_lastProgressPercent;
    int m_progressPercent;
    QMutex m_mutex;
    bool m_cancelled;
};

#endif // ACTIONPROGRESS_H
