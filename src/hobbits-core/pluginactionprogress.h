#ifndef ACTIONPROGRESS_H
#define ACTIONPROGRESS_H

#include <QMutex>
#include <QObject>
#include "hobbits-core_global.h"
#include <QMap>
#include <QVariant>

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

    void sendUpdate(QString type, QVariant value);

Q_SIGNALS:
    void progressPercentChanged(int percent);
    void progressUpdate(QString type, QVariant value);

protected:
    int m_lastProgressPercent;
    int m_progressPercent;
    QMutex m_mutex;
    bool m_cancelled;
};

#endif // ACTIONPROGRESS_H
