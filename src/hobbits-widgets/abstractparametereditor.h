#ifndef ABSTRACTPARAMETEREDITOR_H
#define ABSTRACTPARAMETEREDITOR_H

#include <QWidget>
#include "bitcontainerpreview.h"
#include "pluginactionprogress.h"
#include "displayhandle.h"

class AbstractParameterEditor : public QWidget
{
    Q_OBJECT

public:
    virtual QString title() = 0;

    virtual bool setParameters(QJsonObject parameters) = 0;
    virtual QJsonObject parameters() = 0;

    /// If true, this editor does not require additional accept and reject buttons to behave like a dialog
    virtual bool isStandaloneDialog() { return false; }

    /// Synchronized call to previewBitsImpl() and UI-thread-bound previewBitsUiImpl()
    void previewBits(QSharedPointer<BitContainerPreview> container,
                             QSharedPointer<PluginActionProgress> progress)
    {
        QMutexLocker(&this->m_previewLock);
        previewBitsImpl(container, progress);
        QMetaObject::invokeMethod(this, "previewBitsUiImpl", Qt::QueuedConnection, Q_ARG(QSharedPointer<BitContainerPreview>, container));
    }

    virtual void giveDisplayHandle(QSharedPointer<DisplayHandle> handle)
    {
        m_displayHandle = handle;
    }

signals:
    void accepted();
    void rejected();

protected slots:
    /**
     * Can be reimplemented to make limited metadata adjustments to the container. It can be run on a separate
     * thread from the main UI thread, so all interactions with UI elements should happen in previewBitsUiImpl(),
     * which will safely be called in the UI thread after this method completes.
     */
    virtual void previewBitsImpl(QSharedPointer<BitContainerPreview> container,
                             QSharedPointer<PluginActionProgress> progress)
    {
        Q_UNUSED(container)
        Q_UNUSED(progress)
    }

    /**
     * Can be reimplemented to adjust UI to current container. Time consuming pre-processing should be done in
     * previewBitsImpl(), which runs in a separate thread (but cannot directly affect UI elements)
     */
    virtual void previewBitsUiImpl(QSharedPointer<BitContainerPreview> container)
    {
        Q_UNUSED(container)
    }

protected:
    QSharedPointer<DisplayHandle> m_displayHandle;

private:
    QMutex m_previewLock;
};

#endif // ABSTRACTPARAMETEREDITOR_H
