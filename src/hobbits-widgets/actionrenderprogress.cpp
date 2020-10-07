#include "actionrenderprogress.h"

ActionRenderProgress::ActionRenderProgress() :
    PluginActionProgress()
{

}

void ActionRenderProgress::setRenderPreview(QImage preview)
{
    QMutexLocker locker(&m_mutex);

    m_preview = preview;
    emit renderPreviewChanged(m_preview);
}
