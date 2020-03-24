#ifndef ACTIONPROGRESS_H
#define ACTIONPROGRESS_H

#include <QMutex>
#include <QObject>

#include "hobbits-core_global.h"

class HOBBITSCORESHARED_EXPORT ActionProgress : public QObject
{
    Q_OBJECT

public:
    ActionProgress(QObject *parent = nullptr);

    void setProgressPercent(int progressPercent);
    void setCancelled(bool cancelled);
    bool getCancelled();

signals:
    void progressPercentChanged(int);

private:
    int m_lastProgressPercent;
    int m_progressPercent;
    QMutex m_mutex;
    bool m_cancelled;
};

#endif // ACTIONPROGRESS_H
