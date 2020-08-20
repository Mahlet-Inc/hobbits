#ifndef ACTIONWATCHER_H
#define ACTIONWATCHER_H

#include "actionprogress.h"
#include <QFuture>
#include <QFutureWatcher>
#include <QObject>

template<class T>
class Q_DECL_EXPORT ActionWatcher
{
public:
    explicit ActionWatcher(QFuture<T> future, QSharedPointer<ActionProgress> progress) :
        m_progress(progress)
    {
        m_future = future;
        m_futureWatcher.setFuture(future);
    }

    T result()
    {
        return m_future.result();
    }

    QSharedPointer<ActionProgress> progress()
    {
        return m_progress;
    }

    QFutureWatcher<T>* watcher()
    {
        return &m_futureWatcher;
    }

private:
    QFuture<T> m_future;
    QFutureWatcher<T> m_futureWatcher;
    QSharedPointer<ActionProgress> m_progress;
};

#endif // ACTIONWATCHER_H
