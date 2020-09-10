#include "previewscrollbar.h"
#include <QPainter>
#include <QtConcurrent/QtConcurrentRun>
#include "settingsmanager.h"
#include <QMouseEvent>

PreviewScrollBar::PreviewScrollBar(QWidget *parent) : QWidget(parent)
{
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Expanding);
    setMinimumWidth(50);
}

PreviewScrollBar::~PreviewScrollBar()
{
    for (auto ptr: m_renderWatchers.keys()) {
        auto watcher = m_renderWatchers.value(ptr);
        watcher->progress()->setCancelled(true);
        watcher->watcher()->waitForFinished();
    }
}

int PreviewScrollBar::getFrameOffset()
{
    return m_frameOffset;
}

void PreviewScrollBar::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.fillRect(QRect(QPoint(0,0), this->size()), Qt::darkGray);

    if (m_manager.isNull() || m_manager->getCurrentContainer().isNull()) {
        return;
    }

    auto container = m_manager->getCurrentContainer();
    auto containerPtr = reinterpret_cast<quint64>(container.data());

    if (!m_imageCache.contains(containerPtr)) {
        if (m_previewImageCache.contains(containerPtr)) {
            QImage preview = m_previewImageCache.value(containerPtr);
            painter.drawImage(QRect(QPoint(0,0), this->size()), preview);
        }
        else if (!m_renderWatchers.contains(containerPtr)) {
            QSharedPointer<ActionRenderProgress> progress(new ActionRenderProgress());
            QFuture<QImage> future = QtConcurrent::run(QThreadPool::globalInstance(),
                                                       PreviewScrollBar::renderPreview,
                                                       container, progress);

            auto actionWatcher = QSharedPointer<ActionWatcher<QImage>>(
                    new ActionWatcher<QImage>(future, progress));
            m_renderWatchers.insert(containerPtr, actionWatcher);

            auto watcherRef = QWeakPointer<ActionWatcher<QImage>>(actionWatcher);

            connect(progress.data(), &ActionRenderProgress::renderPreviewChanged, this, [containerPtr, watcherRef, this](const QImage& preview) {
                if (watcherRef.isNull()) {
                    return;
                }
                m_previewImageCache.remove(containerPtr);
                m_previewImageCache.insert(containerPtr, QImage(preview));
                this->repaint();
            }, Qt::QueuedConnection);

            connect(container.data(), &BitContainer::changed, this, [containerPtr, watcherRef, this]() {
                auto watcher = watcherRef.toStrongRef();
                if (watcherRef.isNull()) {
                    return;
                }
                watcher->progress()->setCancelled(true);
                m_imageCache.remove(containerPtr);
                m_previewImageCache.remove(containerPtr);
                m_renderWatchers.remove(containerPtr);
                this->repaint();
            });

            connect(actionWatcher->watcher(), &QFutureWatcher<QImage>::finished, this, [containerPtr, watcherRef, this]() {
                auto watcher = watcherRef.toStrongRef();
                if (watcherRef.isNull()) {
                    return;
                }
                QImage result = watcher->result();
                if (m_renderWatchers.value(containerPtr) == watcher) {
                    if (!result.isNull()) {
                        m_imageCache.remove(containerPtr);
                        m_imageCache.insert(containerPtr, result);
                    }
                    m_renderWatchers.remove(containerPtr);
                }
                this->repaint();
            });

            // clean out any deleted containers
            m_weakRefMap.insert(containerPtr, QWeakPointer<BitContainer>(container));
            for (quint64 key : m_weakRefMap.keys()) {
                if (m_weakRefMap.value(key).isNull()) {
                    m_weakRefMap.remove(key);
                    m_imageCache.remove(key);
                    m_previewImageCache.remove(key);
                    m_renderWatchers.remove(key);
                }
            }
        }
    }
    else {
        QImage preview = m_imageCache.value(containerPtr);
        painter.drawImage(QRect(QPoint(0,0), this->size()), preview);
    }

    // draw frame offset
    painter.setCompositionMode(QPainter::CompositionMode_Difference);
    painter.setPen(QPen(QBrush(Qt::white), 2));
    int pos = int(this->height() * double(m_frameOffset) / double(container->frameCount()));
    painter.drawLine(0, pos, this->width(), pos);
}

void PreviewScrollBar::mouseMoveEvent(QMouseEvent *event)
{
    if (m_mousePressing) {
        getOffsetFromEvent(event);
    }
}

void PreviewScrollBar::mousePressEvent(QMouseEvent *event)
{
    m_mousePressing = true;
    getOffsetFromEvent(event);
}

void PreviewScrollBar::mouseReleaseEvent(QMouseEvent *event)
{
    Q_UNUSED(event)
    m_mousePressing = false;
}

void PreviewScrollBar::leaveEvent(QEvent *event)
{
    Q_UNUSED(event)
    m_mousePressing = false;
}

void PreviewScrollBar::getOffsetFromEvent(QMouseEvent* event)
{
    if (m_manager.isNull() || m_manager->getCurrentContainer().isNull()) {
        return;
    }
    double percent = double(event->y()) / this->height();
    setFrameOffset(int(double(m_manager->getCurrentContainer()->frameCount()) * percent));
}

void PreviewScrollBar::setFrameOffset(int offset)
{
    if (offset != m_frameOffset) {
        m_frameOffset = offset;
        if (!m_displayHandle.isNull() && m_frameOffset != m_displayHandle->getFrameOffset()) {
            m_displayHandle->setOffsets(m_displayHandle->getBitOffset(), m_frameOffset);
        }
        emit frameOffsetChanged(m_frameOffset);
        repaint();
    }
}

void PreviewScrollBar::setBitContainerManager(QSharedPointer<BitContainerManager> manager)
{
    if (!m_manager.isNull()) {
        disconnect(m_manager.data(), SIGNAL(currSelectionChanged(QSharedPointer<BitContainer>, QSharedPointer<BitContainer>)), this, SLOT(repaint()));
    }

    m_manager = manager;

    connect(m_manager.data(), SIGNAL(currSelectionChanged(QSharedPointer<BitContainer>, QSharedPointer<BitContainer>)), this, SLOT(repaint()));

    repaint();
}


void PreviewScrollBar::setDisplayHandle(QSharedPointer<DisplayHandle> displayHandle)
{
    if (!m_displayHandle.isNull()) {
        disconnect(m_displayHandle.data(), &DisplayHandle::newOffsets, this, &PreviewScrollBar::checkDisplayHandleOffset);
    }

    m_displayHandle = displayHandle;

    connect(m_displayHandle.data(), &DisplayHandle::newOffsets, this, &PreviewScrollBar::checkDisplayHandleOffset);
    checkDisplayHandleOffset();
}

void PreviewScrollBar::checkDisplayHandleOffset()
{
    if (m_displayHandle.isNull()) {
        return;
    }

    setFrameOffset(m_displayHandle->getFrameOffset());
}

void PreviewScrollBar::newContainer()
{
    repaint();
}

QImage PreviewScrollBar::renderPreview(QSharedPointer<BitContainer> container, QSharedPointer<ActionRenderProgress> progress)
{
    if (container->info()->maxFrameWidth() < 8 || container->frameCount() < 1) {
        return QImage();
    }
    auto frames = container->info()->frames();
    int width = int(container->info()->maxFrameWidth() / 8);
    int height = qMin(10000ll, frames->size());
    QImage image(width, height, QImage::Format_ARGB32);
    image.fill(qRgb(50, 50, 90));
    QPainter imagePainter(&image);
    imagePainter.setRenderHint(QPainter::Antialiasing, true);

    QColor c = SettingsManager::getInstance().getUiSetting(SettingsData::BYTE_HUE_SAT_KEY).value<QColor>();
    int hue = c.hue();
    int saturation = c.saturation();
    qint64 chunkHeight = qMax(50, qMin(10000, 5000000/width));
    int chunkSize = qMin(frames->size(), chunkHeight);
    double chunkHeightRatio = double(chunkSize)/double(frames->size());
    double targetChunkHeight = chunkHeightRatio * height;
    QImage bufferChunk(width, chunkSize, QImage::Format_ARGB32);
    for (int frame = 0; frame < frames->size(); frame += chunkSize) {
        bufferChunk.fill(qRgb(0, 0, 0));
        int offset = 0;
        for (; offset < chunkSize && offset + frame < frames->size(); offset++) {
            Frame f = Frame(container->bits(), frames->at(offset + frame));
            qint64 byteOffset = f.start()/8;
            for (int i = 0; i < f.size()/8 && byteOffset + i < container->bits()->sizeInBytes(); i++) {
                char byteValue = container->bits()->byteAt(byteOffset + i);
                c.setHsl(hue, saturation, reinterpret_cast<quint8&>(byteValue));
                bufferChunk.setPixel(i, offset, c.rgba());
            }
        }
        double heightRatio = double(offset)/double(chunkSize);
        QRectF target(0, targetChunkHeight * (frame / chunkSize), width, heightRatio * targetChunkHeight);
        QRectF source(0, 0, width, offset);
        imagePainter.drawImage(target, bufferChunk, source);
        if (progress->getCancelled()) {
            return QImage();
        }
        progress->setRenderPreview(image);
    }

    return image;
}
