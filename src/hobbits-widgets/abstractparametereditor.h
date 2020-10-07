#ifndef ABSTRACTPARAMETEREDITOR_H
#define ABSTRACTPARAMETEREDITOR_H

#include <QWidget>
#include "bitcontainerpreview.h"
#include "pluginactionprogress.h"

class AbstractParameterEditor : public QWidget
{
    Q_OBJECT

public:
    virtual QString title() = 0;

    virtual bool setParameters(QJsonObject parameters) = 0;
    virtual QJsonObject parameters() = 0;

    /// If true, this editor does not require additional accept and reject buttons to behave like a dialog
    virtual bool isStandaloneDialog() { return false; }

    /**
     * Can be reimplemented to adjust UI to current container or make limited metadata adjustments.
     * It can be run on a separate thread from the UI, so all interactions with UI elements must happen
     * in a separate method that is triggered via Qt::QueuedConnection
     */
    virtual void previewBits(QSharedPointer<BitContainerPreview> container,
                             QSharedPointer<PluginActionProgress> progress)
    {
        Q_UNUSED(container)
        Q_UNUSED(progress)
    }

signals:
    void accepted();
    void rejected();
};

#endif // ABSTRACTPARAMETEREDITOR_H
