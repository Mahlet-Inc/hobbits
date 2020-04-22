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

int PreviewScrollBar::getFrameOffset()
{
    return m_frameOffset;
}

void PreviewScrollBar::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.fillRect(QRect(QPoint(0,0), this->size()), Qt::black);

    if (m_manager.isNull() || m_manager->getCurrentContainer().isNull()) {
        return;
    }

    auto container = m_manager->getCurrentContainer();
    auto containerPtr = reinterpret_cast<quint64>(container.data());

    if (!m_imageCache.contains(containerPtr)) {
        if (!m_renderWatchers.contains(container)) {
            QSharedPointer<ActionRenderProgress> progress(new ActionRenderProgress());
            QFuture<QImage> future = QtConcurrent::run(QThreadPool::globalInstance(),
                                                       PreviewScrollBar::renderPreview,
                                                       container, progress);

            connect(progress.data(), &ActionRenderProgress::renderPreviewChanged, this, [containerPtr, this](const QImage& preview) {
                m_imageCache.remove(containerPtr);
                m_imageCache.insert(containerPtr, QImage(preview));
                this->repaint();
            }, Qt::QueuedConnection);

            auto actionWatcher = QSharedPointer<ActionWatcher<QImage>>(
                    new ActionWatcher<QImage>(future, progress));

            m_renderWatchers.insert(container, actionWatcher);
            connect(actionWatcher->watcher(), SIGNAL(finished()), this, SLOT(checkRenderWatchers()));
        }
    }
    else {
        QImage preview = m_imageCache.value(containerPtr);
        painter.drawImage(QRect(QPoint(0,0), this->size()), preview);
    }

    // draw frame offset
    painter.setCompositionMode(QPainter::CompositionMode_Difference);
    painter.setPen(QPen(QBrush(Qt::white), 2));
    int pos = int(this->height() * double(m_frameOffset) / double(container->frames().size()));
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
    m_mousePressing = false;
}

void PreviewScrollBar::leaveEvent(QEvent *event)
{
    m_mousePressing = false;
}


void PreviewScrollBar::getOffsetFromEvent(QMouseEvent* event)
{
    if (m_manager.isNull() || m_manager->getCurrentContainer().isNull()) {
        return;
    }
    double percent = double(event->y()) / this->height();
    setFrameOffset(int(double(m_manager->getCurrentContainer()->frames().size()) * percent));
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

void PreviewScrollBar::checkRenderWatchers()
{
    QList<QSharedPointer<BitContainer>> removals;
    for (auto container: m_renderWatchers.keys()) {
        if (m_renderWatchers.value(container)->watcher()->isFinished()) {
            auto containerPtr = reinterpret_cast<quint64>(container.data());
            removals.append(container);
            m_imageCache.remove(containerPtr);
            m_imageCache.insert(containerPtr, m_renderWatchers.value(container)->result());
        }
    }

    for (auto removal : removals) {
        m_renderWatchers.remove(removal);
    }

    // TODO: clean up cache using weak ref checks and least-recently-used size limit

    repaint();
}

QImage PreviewScrollBar::renderPreview(QSharedPointer<BitContainer> container, QSharedPointer<ActionRenderProgress> progress)
{
    int width = int(container->bitInfo()->maxFrameWidth() / 8);
    int height = qMin(10000, container->frames().size());
    QImage image(width, height, QImage::Format_ARGB32);
    image.fill(qRgb(50, 50, 90));
    QPainter imagePainter(&image);
    imagePainter.setRenderHint(QPainter::Antialiasing, true);

    QColor c = SettingsManager::getInstance().getUiSetting(SettingsData::BYTE_HUE_SAT_KEY).value<QColor>();
    int hue = c.hue();
    int saturation = c.saturation();
    int chunkSize = qMin(container->frames().size(), 10000);
    double chunkHeightRatio = double(chunkSize)/double(container->frames().size());
    double targetChunkHeight = chunkHeightRatio * height;
    QImage bufferChunk(width, chunkSize, QImage::Format_ARGB32);
    for (int frame = 0; frame < container->frames().size(); frame += chunkSize) {
        bufferChunk.fill(qRgb(0, 0, 0));
        int offset = 0;
        for (; offset < chunkSize && offset + frame < container->frames().size(); offset++) {
            Frame f = container->frames().at(offset + frame);
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
        progress->setRenderPreview(image);
    }

    return image;
}
