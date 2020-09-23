#ifndef ACTIONRENDERPROGRESS_H
#define ACTIONRENDERPROGRESS_H

#include "actionprogress.h"
#include <QImage>
#include "hobbits-widgets_global.h"

class HOBBITSWIDGETSSHARED_EXPORT ActionRenderProgress : public ActionProgress
{
    Q_OBJECT
public:
    ActionRenderProgress();

    void setRenderPreview(QImage preview);

signals:
    void renderPreviewChanged(const QImage&);

private:
    QImage m_preview;
};

#endif // ACTIONRENDERPROGRESS_H
