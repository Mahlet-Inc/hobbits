#include "pluginactionprogress.h"
#include <QMutexLocker>
#include <QVariant>

PluginActionProgress::PluginActionProgress(QObject *parent) :
    QObject(parent),
    m_lastProgressPercent(0),
    m_progressPercent(0),
    m_cancelled(0)
{

}

void PluginActionProgress::setProgressPercent(int progressPercent)
{
    QMutexLocker locker(&m_mutex);
    if (progressPercent != m_lastProgressPercent) {
        m_lastProgressPercent = progressPercent;
        m_progressPercent = progressPercent;
        emit progressPercentChanged(m_progressPercent);
    }
}

void PluginActionProgress::setCancelled(bool cancelled)
{
    QMutexLocker locker(&m_mutex);

    m_cancelled = cancelled;
}

bool PluginActionProgress::isCancelled()
{
    QMutexLocker locker(&m_mutex);

    return m_cancelled;
}

void PluginActionProgress::setProgress(qint64 completed, qint64 required)
{
    int percent = int(double(completed) / double(required) * 100.0);
    setProgressPercent(percent);
}

void PluginActionProgress::setProgress(int completed, int required)
{
    int percent = int(double(completed) / double(required) * 100.0);
    setProgressPercent(percent);
}

void PluginActionProgress::setProgress(double completed, double required)
{
    int percent = int(completed / required * 100.0);
    setProgressPercent(percent);
}

void PluginActionProgress::sendUpdate(QString type, QVariant value)
{
    QMutexLocker locker(&m_mutex);
    emit progressUpdate(type, value);
}
