#include "actionrenderprogress.h"

ActionRenderProgress::ActionRenderProgress() :
    ActionProgress()
{

}

void ActionRenderProgress::setRenderPreview(QImage preview)
{
    QMutexLocker locker(&m_mutex);

    m_preview = preview;
    emit renderPreviewChanged(m_preview);
}
